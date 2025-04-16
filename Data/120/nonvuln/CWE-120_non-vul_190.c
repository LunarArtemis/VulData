rad_dict_vendor_t *rad_dict_find_vendor_name(const char *name)
{
	struct rad_dict_vendor_t *vendor;

	list_for_each_entry(vendor, &dict->vendors, entry) {
		if (!strcmp(vendor->name, name))
			return vendor;
	}

	return NULL;
}