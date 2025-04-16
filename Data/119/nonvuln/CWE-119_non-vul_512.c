static int check_mapped_selector_name(struct mixer_build *state, int unitid,
				      int index, char *buf, int buflen)
{
	const struct usbmix_selector_map *p;

	if (!state->selector_map)
		return 0;
	for (p = state->selector_map; p->id; p++) {
		if (p->id == unitid && index < p->count)
			return strlcpy(buf, p->names[index], buflen);
	}
	return 0;
}