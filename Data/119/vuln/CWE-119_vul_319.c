static int 
mpeg_resync(wtap *wth, int *err, gchar **err_info _U_)
{
	gint64 offset = file_tell(wth->fh);
	int count = 0;
	int byte = file_getc(wth->fh);

	while (byte != EOF) {
		if (byte == 0xff && count > 0) {
			byte = file_getc(wth->fh);
			if (byte != EOF && (byte & 0xe0) == 0xe0)
				break;
		} else
			byte = file_getc(wth->fh);
		count++;
	}
	if (file_seek(wth->fh, offset, SEEK_SET, err) == -1)
		return 0;
	return count;
}