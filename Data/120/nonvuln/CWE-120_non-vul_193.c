rad_dict_attr_t *rad_dict_find_attr(const char *name)
{
	return dict_find_attr(&dict->items, name);
}