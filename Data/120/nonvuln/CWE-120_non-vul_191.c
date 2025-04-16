rad_dict_vendor_t *rad_dict_find_vendor_id(int id)
{
	struct rad_dict_vendor_t *vendor;

	list_for_each_entry(vendor, &dict->vendors, entry) {
		if (vendor->id == id)
			return vendor;
	}

	return NULL;
}