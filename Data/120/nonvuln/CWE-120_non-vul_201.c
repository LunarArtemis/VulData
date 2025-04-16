rad_dict_attr_t *rad_dict_find_vendor_attr(struct rad_dict_vendor_t *vendor, const char *name)
{
	return dict_find_attr(&vendor->items, name);
}