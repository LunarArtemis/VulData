static struct rad_dict_attr_t *dict_find_attr(struct list_head *items, const char *name)
{
	struct rad_dict_attr_t *attr;

	list_for_each_entry(attr, items, entry)
		if (!strcmp(attr->name, name))
			return attr;

	return NULL;
}