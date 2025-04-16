int rad_dict_load(const char *fname)
{
	int r = -1;

	if (!dict) {
		dict = malloc(sizeof(*dict));

		if (!dict) {
			log_emerg("radius: out of memory\n");
			return -1;
		}
		INIT_LIST_HEAD(&dict->items);
		INIT_LIST_HEAD(&dict->vendors);
	}

	path = _malloc(PATH_MAX);
	if (!path) {
		log_emerg("radius: out of memory\n");
		goto out_free_dict;
	}

	fname1 = _malloc(PATH_MAX);
	if (!fname1) {
		log_emerg("radius: out of memory\n");
		goto out_free_path;
	}

	buf = _malloc(BUF_SIZE);
	if (!buf) {
		log_emerg("radius: out of memory\n");
		goto out_free_fname1;
	}

	strcpy(path, fname);

	r = dict_load(fname);

out_free_fname1:
	_free(fname1);
out_free_path:
	_free(path);
out_free_dict:
	if (r)
		rad_dict_free(dict);
	return r;
}