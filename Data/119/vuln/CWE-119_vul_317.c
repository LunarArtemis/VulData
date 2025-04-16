static int 
mpeg_read_header(wtap *wth, int *err, gchar **err_info, guint32 *n)
{
	int bytes_read;

	errno = WTAP_ERR_CANT_READ;
	bytes_read = file_read(n, sizeof *n, wth->fh);
	if (bytes_read != sizeof *n) {
		*err = file_error(wth->fh, err_info);
		if (*err == 0 && bytes_read != 0)
			*err = WTAP_ERR_SHORT_READ;
		return -1;
	}
	*n = g_ntohl(*n);
	if (file_seek(wth->fh, -(gint64)(sizeof *n), SEEK_CUR, err) == -1)
		return -1;
	return bytes_read;
}