void rad_dict_free(struct rad_dict_t *dict)
{
	struct rad_dict_attr_t *attr;
	struct rad_dict_value_t *val;

	while (!list_empty(&dict->items)) {
		attr = list_entry(dict->items.next, typeof(*attr), entry);
		while (!list_empty(&attr->values)) {
			val = list_entry(attr->values.next, typeof(*val), entry);
			list_del(&val->entry);
			_free((char*)val->name);
			if (attr->type == ATTR_TYPE_STRING)
				_free((char*)val->val.string);
			_free(val);
		}
		list_del(&attr->entry);
		_free((char*)attr->name);
		_free(attr);
	}
	free(dict);
}