rad_dict_attr_t *rad_dict_find_attr_id(struct rad_dict_vendor_t *vendor, int id)
{
	struct rad_dict_attr_t *attr;
	struct list_head *items = vendor ? &vendor->items : &dict->items;

	list_for_each_entry(attr, items, entry)
		if (attr->id == id)
			return attr;

	return NULL;
}