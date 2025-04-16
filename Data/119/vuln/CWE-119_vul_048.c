static struct nfp_cpp_area_cache *
area_cache_get(struct nfp_cpp *cpp, u32 id,
	       u64 addr, unsigned long *offset, size_t length)
{
	struct nfp_cpp_area_cache *cache;
	int err;

	/* Early exit when length == 0, which prevents
	 * the need for special case code below when
	 * checking against available cache size.
	 */
	if (length == 0 || id == 0)
		return NULL;

	/* Remap from cpp_island to cpp_target */
	err = nfp_target_cpp(id, addr, &id, &addr, cpp->imb_cat_table);
	if (err < 0)
		return NULL;

	mutex_lock(&cpp->area_cache_mutex);

	if (list_empty(&cpp->area_cache_list)) {
		mutex_unlock(&cpp->area_cache_mutex);
		return NULL;
	}

	addr += *offset;

	/* See if we have a match */
	list_for_each_entry(cache, &cpp->area_cache_list, entry) {
		if (id == cache->id &&
		    addr >= cache->addr &&
		    addr + length <= cache->addr + cache->size)
			goto exit;
	}

	/* No matches - inspect the tail of the LRU */
	cache = list_entry(cpp->area_cache_list.prev,
			   struct nfp_cpp_area_cache, entry);

	/* Can we fit in the cache entry? */
	if (round_down(addr + length - 1, cache->size) !=
	    round_down(addr, cache->size)) {
		mutex_unlock(&cpp->area_cache_mutex);
		return NULL;
	}

	/* If id != 0, we will need to release it */
	if (cache->id) {
		nfp_cpp_area_release(cache->area);
		cache->id = 0;
		cache->addr = 0;
	}

	/* Adjust the start address to be cache size aligned */
	cache->id = id;
	cache->addr = addr & ~(u64)(cache->size - 1);

	/* Re-init to the new ID and address */
	if (cpp->op->area_init) {
		err = cpp->op->area_init(cache->area,
					 id, cache->addr, cache->size);
		if (err < 0) {
			mutex_unlock(&cpp->area_cache_mutex);
			return NULL;
		}
	}

	/* Attempt to acquire */
	err = nfp_cpp_area_acquire(cache->area);
	if (err < 0) {
		mutex_unlock(&cpp->area_cache_mutex);
		return NULL;
	}

exit:
	/* Adjust offset */
	*offset = addr - cache->addr;
	return cache;
}