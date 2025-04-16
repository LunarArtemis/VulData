rad_dict_value_t *rad_dict_find_val_name(struct rad_dict_attr_t *attr, const char *name)
{
	struct rad_dict_value_t *val;

	list_for_each_entry(val, &attr->values, entry)
		if (!strcmp(val->name, name))
			return val;

	return NULL;
}