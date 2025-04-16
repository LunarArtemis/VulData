static void txt_add(unsigned char *s, int sz)
{
	if (skip)
		return;
	if (offset + sz > size) {
		size = (offset + sz + 8191) / 8192 * 8192;
		if (!dst)
			dst = malloc(size);
		else
			dst = realloc(dst, size);
		if (!dst) {
			fprintf(stderr, "Out of memory - abort\n");
			exit(EXIT_FAILURE);
		}
	}
	memcpy(dst + offset, s, sz);
	offset += sz;
}