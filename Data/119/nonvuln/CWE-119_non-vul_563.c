static const struct usbmix_name_map *
find_map(const struct usbmix_name_map *p, int unitid, int control)
{
	if (!p)
		return NULL;

	for (; p->id; p++) {
		if (p->id == unitid &&
		    (!control || !p->control || control == p->control))
			return p;
	}
	return NULL;
}