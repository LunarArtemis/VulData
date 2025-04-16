rad_dict_value_t *rad_dict_find_val(struct rad_dict_attr_t *attr, rad_value_t v)
{
	struct rad_dict_value_t *val;

	if (attr->type != ATTR_TYPE_INTEGER)
		return NULL;

	list_for_each_entry(val, &attr->values, entry)
		if (val->val.integer == v.integer)
			return val;

	return NULL;
}