int 
mpeg_open(wtap *wth, int *err, gchar **err_info)
{
	int bytes_read;
	char magic_buf[16];
	struct _mpeg_magic* m;
	mpeg_t *mpeg;
	
	errno = WTAP_ERR_CANT_READ;
	bytes_read = file_read(magic_buf, sizeof magic_buf, wth->fh);
	if (bytes_read != (int) sizeof magic_buf) {
		*err = file_error(wth->fh, err_info);
		if (*err != 0)
			return -1;
		return 0;
	}

	for (m=magic; m->match; m++) {
		if (memcmp(magic_buf, m->match, m->len) == 0)
			goto good_magic;
	}
	
	return 0;

good_magic:
	/* This appears to be a file with MPEG data. */
	if (file_seek(wth->fh, 0, SEEK_SET, err) == -1)
		return -1;

	wth->file_type = WTAP_FILE_MPEG;
	wth->file_encap = WTAP_ENCAP_MPEG;
	wth->tsprecision = WTAP_FILE_TSPREC_NSEC;
	wth->subtype_read = mpeg_read;
	wth->subtype_seek_read = mpeg_seek_read;
	wth->snapshot_length = 0;

	mpeg = (mpeg_t *)g_malloc(sizeof(mpeg_t));
	wth->priv = (void *)mpeg;
	mpeg->now.secs = 0;
	mpeg->now.nsecs = 0;
	mpeg->t0 = mpeg->now.secs;

	return 1;
}