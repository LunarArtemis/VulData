GF_Err abst_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_AdobeBootstrapInfoBox *ptr = (GF_AdobeBootstrapInfoBox *)s;
	int i;
	u32 tmp_strsize;
	char *tmp_str;
	GF_Err e;

	ISOM_DECREASE_SIZE(ptr, 25)
	ptr->bootstrapinfo_version = gf_bs_read_u32(bs);
	ptr->profile = gf_bs_read_int(bs, 2);
	ptr->live = gf_bs_read_int(bs, 1);
	ptr->update = gf_bs_read_int(bs, 1);
	ptr->reserved = gf_bs_read_int(bs, 4);
	ptr->time_scale = gf_bs_read_u32(bs);
	ptr->current_media_time = gf_bs_read_u64(bs);
	ptr->smpte_time_code_offset = gf_bs_read_u64(bs);

	i=0;
	if (ptr->size<8) return GF_ISOM_INVALID_FILE;
	tmp_strsize =(u32)ptr->size;
	tmp_str = gf_malloc(sizeof(char)*tmp_strsize);
	if (!tmp_str) return GF_OUT_OF_MEM;
	memset(tmp_str, 0, sizeof(char)*tmp_strsize);

	while (tmp_strsize) {
		ISOM_DECREASE_SIZE(ptr, 1)
		tmp_str[i] = gf_bs_read_u8(bs);
		tmp_strsize--;
		if (!tmp_str[i])
			break;
		i++;
	}
	if (i) {
		ptr->movie_identifier = gf_strdup(tmp_str);
	}

	ISOM_DECREASE_SIZE(ptr, 1)
	ptr->server_entry_count = gf_bs_read_u8(bs);
	for (i=0; i<ptr->server_entry_count; i++) {
		int j=0;
		tmp_strsize=(u32)ptr->size;
		while (tmp_strsize) {
			ISOM_DECREASE_SIZE(ptr, 1)
			tmp_str[j] = gf_bs_read_u8(bs);
			tmp_strsize--;
			if (!tmp_str[j])
				break;
			j++;
		}
		if (j) {
			gf_list_insert(ptr->server_entry_table, gf_strdup(tmp_str), i);
		}
	}

	ISOM_DECREASE_SIZE(ptr, 1)
	ptr->quality_entry_count = gf_bs_read_u8(bs);
	for (i=0; i<ptr->quality_entry_count; i++) {
		int j=0;
		tmp_strsize=(u32)ptr->size;
		while (tmp_strsize) {
			ISOM_DECREASE_SIZE(ptr, 1)
			tmp_str[j] = gf_bs_read_u8(bs);
			tmp_strsize--;
			if (!tmp_str[j])
				break;
			j++;
		}

		if (j) {
			gf_list_insert(ptr->quality_entry_table, gf_strdup(tmp_str), i);
		}
	}

	i=0;
	tmp_strsize=(u32)ptr->size;
	while (tmp_strsize) {
		ISOM_DECREASE_SIZE(ptr, 1)
		tmp_str[i] = gf_bs_read_u8(bs);
		tmp_strsize--;
		if (!tmp_str[i])
			break;
		i++;
	}
	if (i) {
		ptr->drm_data = gf_strdup(tmp_str);
	}

	i=0;
	tmp_strsize=(u32)ptr->size;
	while (tmp_strsize) {
		ISOM_DECREASE_SIZE(ptr, 1)
		tmp_str[i] = gf_bs_read_u8(bs);
		tmp_strsize--;
		if (!tmp_str[i])
			break;
		i++;
	}
	if (i) {
		ptr->meta_data = gf_strdup(tmp_str);
	}

	ISOM_DECREASE_SIZE(ptr, 1)
	ptr->segment_run_table_count = gf_bs_read_u8(bs);
	for (i=0; i<ptr->segment_run_table_count; i++) {
		GF_AdobeSegmentRunTableBox *asrt = NULL;
		e = gf_isom_box_parse((GF_Box **)&asrt, bs);
		if (e) {
			if (asrt) gf_isom_box_del((GF_Box*)asrt);
			gf_free(tmp_str);
			return e;
		}
		gf_list_add(ptr->segment_run_table_entries, asrt);
	}

	ISOM_DECREASE_SIZE(ptr, 1)
	ptr->fragment_run_table_count = gf_bs_read_u8(bs);
	for (i=0; i<ptr->fragment_run_table_count; i++) {
		GF_AdobeFragmentRunTableBox *afrt = NULL;
		e = gf_isom_box_parse((GF_Box **)&afrt, bs);
		if (e) {
			if (afrt) gf_isom_box_del((GF_Box*)afrt);
			gf_free(tmp_str);
			return e;
		}
		gf_list_add(ptr->fragment_run_table_entries, afrt);
	}

	gf_free(tmp_str);

	return GF_OK;
}