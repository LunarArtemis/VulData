static void read_zip_dir_imp(fz_context *ctx, fz_zip_archive *zip, int start_offset)
{
	fz_stream *file = zip->super.file;
	int sig;
	int i, count, offset, csize, usize;
	int namesize, metasize, commentsize;
	char *name;
	size_t n;

	zip->count = 0;

	fz_seek(ctx, file, start_offset, 0);

	sig = fz_read_int32_le(ctx, file);
	if (sig != ZIP_END_OF_CENTRAL_DIRECTORY_SIG)
		fz_throw(ctx, FZ_ERROR_GENERIC, "wrong zip end of central directory signature (0x%x)", sig);

	(void) fz_read_int16_le(ctx, file); /* this disk */
	(void) fz_read_int16_le(ctx, file); /* start disk */
	(void) fz_read_int16_le(ctx, file); /* entries in this disk */
	count = fz_read_int16_le(ctx, file); /* entries in central directory disk */
	(void) fz_read_int32_le(ctx, file); /* size of central directory */
	offset = fz_read_int32_le(ctx, file); /* offset to central directory */

	/* ZIP64 */
	if (count == 0xFFFF || offset == 0xFFFFFFFF)
	{
		int64_t offset64, count64;

		fz_seek(ctx, file, start_offset - 20, 0);

		sig = fz_read_int32_le(ctx, file);
		if (sig != ZIP64_END_OF_CENTRAL_DIRECTORY_LOCATOR_SIG)
			fz_throw(ctx, FZ_ERROR_GENERIC, "wrong zip64 end of central directory locator signature (0x%x)", sig);

		(void) fz_read_int32_le(ctx, file); /* start disk */
		offset64 = fz_read_int64_le(ctx, file); /* offset to end of central directory record */
		if (offset64 > INT32_MAX)
			fz_throw(ctx, FZ_ERROR_GENERIC, "zip64 files larger than 2 GB aren't supported");

		fz_seek(ctx, file, offset64, 0);

		sig = fz_read_int32_le(ctx, file);
		if (sig != ZIP64_END_OF_CENTRAL_DIRECTORY_SIG)
			fz_throw(ctx, FZ_ERROR_GENERIC, "wrong zip64 end of central directory signature (0x%x)", sig);

		(void) fz_read_int64_le(ctx, file); /* size of record */
		(void) fz_read_int16_le(ctx, file); /* version made by */
		(void) fz_read_int16_le(ctx, file); /* version to extract */
		(void) fz_read_int32_le(ctx, file); /* disk number */
		(void) fz_read_int32_le(ctx, file); /* disk number start */
		count64 = fz_read_int64_le(ctx, file); /* entries in central directory disk */
		(void) fz_read_int64_le(ctx, file); /* entries in central directory */
		(void) fz_read_int64_le(ctx, file); /* size of central directory */
		offset64 = fz_read_int64_le(ctx, file); /* offset to central directory */

		if (count == 0xFFFF)
		{
			if (count64 > INT32_MAX)
				fz_throw(ctx, FZ_ERROR_GENERIC, "zip64 files larger than 2 GB aren't supported");
			count = count64;
		}
		if (offset == 0xFFFFFFFF)
		{
			if (offset64 > INT32_MAX)
				fz_throw(ctx, FZ_ERROR_GENERIC, "zip64 files larger than 2 GB aren't supported");
			offset = offset64;
		}
	}

	fz_seek(ctx, file, offset, 0);

	for (i = 0; i < count; i++)
	{
		sig = fz_read_int32_le(ctx, file);
		if (sig != ZIP_CENTRAL_DIRECTORY_SIG)
			fz_throw(ctx, FZ_ERROR_GENERIC, "wrong zip central directory signature (0x%x)", sig);

		(void) fz_read_int16_le(ctx, file); /* version made by */
		(void) fz_read_int16_le(ctx, file); /* version to extract */
		(void) fz_read_int16_le(ctx, file); /* general */
		(void) fz_read_int16_le(ctx, file); /* method */
		(void) fz_read_int16_le(ctx, file); /* last mod file time */
		(void) fz_read_int16_le(ctx, file); /* last mod file date */
		(void) fz_read_int32_le(ctx, file); /* crc-32 */
		csize = fz_read_int32_le(ctx, file);
		usize = fz_read_int32_le(ctx, file);
		namesize = fz_read_int16_le(ctx, file);
		metasize = fz_read_int16_le(ctx, file);
		commentsize = fz_read_int16_le(ctx, file);
		(void) fz_read_int16_le(ctx, file); /* disk number start */
		(void) fz_read_int16_le(ctx, file); /* int file atts */
		(void) fz_read_int32_le(ctx, file); /* ext file atts */
		offset = fz_read_int32_le(ctx, file);

		name = fz_malloc(ctx, namesize + 1);
		n = fz_read(ctx, file, (unsigned char*)name, namesize);
		if (n < (size_t)namesize)
			fz_throw(ctx, FZ_ERROR_GENERIC, "premature end of data in zip entry name");
		name[namesize] = '\0';

		while (metasize > 0)
		{
			int type = fz_read_int16_le(ctx, file);
			int size = fz_read_int16_le(ctx, file);
			if (type == ZIP64_EXTRA_FIELD_SIG)
			{
				int sizeleft = size;
				if (usize == 0xFFFFFFFF && sizeleft >= 8)
				{
					usize = fz_read_int64_le(ctx, file);
					sizeleft -= 8;
				}
				if (csize == 0xFFFFFFFF && sizeleft >= 8)
				{
					csize = fz_read_int64_le(ctx, file);
					sizeleft -= 8;
				}
				if (offset == 0xFFFFFFFF && sizeleft >= 8)
				{
					offset = fz_read_int64_le(ctx, file);
					sizeleft -= 8;
				}
				fz_seek(ctx, file, sizeleft - size, 1);
			}
			fz_seek(ctx, file, size, 1);
			metasize -= 4 + size;
		}
		if (usize < 0 || csize < 0 || offset < 0)
			fz_throw(ctx, FZ_ERROR_GENERIC, "zip64 files larger than 2 GB are not supported");

		fz_seek(ctx, file, commentsize, 1);

		zip->entries = fz_resize_array(ctx, zip->entries, zip->count + 1, sizeof *zip->entries);

		zip->entries[zip->count].name = name;
		zip->entries[zip->count].offset = offset;
		zip->entries[zip->count].csize = csize;
		zip->entries[zip->count].usize = usize;

		zip->count++;
	}
}