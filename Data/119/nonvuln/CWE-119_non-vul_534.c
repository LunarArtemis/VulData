static inline int
check_ignored_ctl(const struct usbmix_name_map *p)
{
	if (!p || p->name || p->dB)
		return 0;
	return 1;
}