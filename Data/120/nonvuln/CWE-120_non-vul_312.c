static struct fuse_req *request_find(struct fuse_conn *fc, u64 unique)
{
	struct list_head *entry;

	list_for_each(entry, &fc->processing) {
		struct fuse_req *req;
		req = list_entry(entry, struct fuse_req, list);
		if (req->in.h.unique == unique || req->intr_unique == unique)
			return req;
	}
	return NULL;
}