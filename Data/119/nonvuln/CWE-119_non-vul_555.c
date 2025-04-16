static int
check_mapped_name(const struct usbmix_name_map *p, char *buf, int buflen)
{
	if (!p || !p->name)
		return 0;

	buflen--;
	return strlcpy(buf, p->name, buflen);
}