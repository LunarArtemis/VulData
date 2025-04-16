GF_EXPORT
GF_Err gf_isom_get_user_data(GF_ISOFile *movie, u32 trackNumber, u32 UserDataType, bin128 UUID, u32 UserDataIndex, u8 **userData, u32 *userDataSize)
{
	GF_UserDataMap *map;
	GF_UnknownBox *ptr;
	GF_BitStream *bs;
	u32 i;
	bin128 t;
	GF_TrackBox *trak;
	GF_UserDataBox *udta;

	if (!movie || !movie->moov) return GF_BAD_PARAM;

	if (trackNumber) {
		trak = gf_isom_get_track_from_file(movie, trackNumber);
		if (!trak) return GF_BAD_PARAM;
		udta = trak->udta;
	} else {
		if (!movie->moov) return GF_BAD_PARAM;
		udta = movie->moov->udta;
	}
	if (!udta) return GF_BAD_PARAM;

	if (UserDataType == GF_ISOM_BOX_TYPE_UUID) UserDataType = 0;
	memset(t, 1, 16);

	if (!userData || !userDataSize || *userData) return GF_BAD_PARAM;

	i=0;
	while ((map = (GF_UserDataMap*)gf_list_enum(udta->recordList, &i))) {
		if ((map->boxType == GF_ISOM_BOX_TYPE_UUID) && !memcmp(map->uuid, UUID, 16)) goto found;
		else if (map->boxType == UserDataType) goto found;

	}
	return GF_BAD_PARAM;

found:
	if (UserDataIndex) {
		if (UserDataIndex > gf_list_count(map->boxes) ) return GF_BAD_PARAM;
		ptr = (GF_UnknownBox*)gf_list_get(map->boxes, UserDataIndex-1);

		if (ptr->type == GF_ISOM_BOX_TYPE_UNKNOWN) {
			if (!ptr->dataSize) {
				*userData = NULL;
				*userDataSize = 0;
				return GF_OK;
			}
			*userData = (char *)gf_malloc(sizeof(char)*ptr->dataSize);
			if (!*userData) return GF_OUT_OF_MEM;
			memcpy(*userData, ptr->data, sizeof(char)*ptr->dataSize);
			*userDataSize = ptr->dataSize;
			return GF_OK;
		} else if (ptr->type == GF_ISOM_BOX_TYPE_UUID) {
			GF_UnknownUUIDBox *p_uuid = (GF_UnknownUUIDBox *)ptr;
			if (!p_uuid->dataSize) {
				*userData = NULL;
				*userDataSize = 0;
				return GF_OK;
			}
			*userData = (char *)gf_malloc(sizeof(char)*p_uuid->dataSize);
			if (!*userData) return GF_OUT_OF_MEM;
			memcpy(*userData, p_uuid->data, sizeof(char)*p_uuid->dataSize);
			*userDataSize = p_uuid->dataSize;
			return GF_OK;
		} else {
			char *str = NULL;
			switch (ptr->type) {
			case GF_ISOM_BOX_TYPE_NAME:
			//case GF_QT_BOX_TYPE_NAME: same as above
				str = ((GF_NameBox *)ptr)->string;
				break;
			case GF_ISOM_BOX_TYPE_KIND:
				str = ((GF_KindBox *)ptr)->value;
				break;
			}
			if (str) {
				u32 len = (u32) strlen(str) + 1;
				*userData = (char *)gf_malloc(sizeof(char) * len);
				if (!*userData) return GF_OUT_OF_MEM;
				memcpy(*userData, str, sizeof(char)*len);
				*userDataSize = len;
				return GF_OK;
			}
			return GF_NOT_SUPPORTED;
		}
	}

	//serialize all boxes
	bs = gf_bs_new(NULL, 0, GF_BITSTREAM_WRITE);
	i=0;
	while ( (ptr = (GF_UnknownBox*)gf_list_enum(map->boxes, &i))) {
		u32 type, s, data_size;
		char *data=NULL;
		if (ptr->type == GF_ISOM_BOX_TYPE_UNKNOWN) {
			type = ptr->original_4cc;
			data_size = ptr->dataSize;
			data = ptr->data;
		} else if (ptr->type == GF_ISOM_BOX_TYPE_UUID) {
			GF_UnknownUUIDBox *p_uuid = (GF_UnknownUUIDBox *)ptr;
			type = p_uuid->type;
			data_size = p_uuid->dataSize;
			data = p_uuid->data;
		} else {
#ifndef GPAC_DISABLE_ISOM_WRITE
			gf_isom_box_write((GF_Box *)ptr, bs);
#else
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("ISOBMF: udta is a box-list - cannot export in read-only version of libisom in GPAC\n" ));
#endif
			continue;
		}
		s = data_size+8;
		if (ptr->type==GF_ISOM_BOX_TYPE_UUID) s += 16;

		gf_bs_write_u32(bs, s);
		gf_bs_write_u32(bs, type);
		if (type==GF_ISOM_BOX_TYPE_UUID) gf_bs_write_data(bs, (char *) map->uuid, 16);
		if (data) {
			gf_bs_write_data(bs, data, data_size);
		} else if (ptr->child_boxes) {
#ifndef GPAC_DISABLE_ISOM_WRITE
			gf_isom_box_array_write((GF_Box *)ptr, ptr->child_boxes, bs);
#else
			GF_LOG(GF_LOG_WARNING, GF_LOG_CONTAINER, ("ISOBMF: udta is a box-list - cannot export in read-only version of libisom in GPAC\n" ));
#endif
		}
	}
	gf_bs_get_content(bs, userData, userDataSize);
	gf_bs_del(bs);
	return GF_OK;
}