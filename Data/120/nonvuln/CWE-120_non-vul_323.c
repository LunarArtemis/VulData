static void flush_bg_queue(struct fuse_conn *fc)
{
	while (fc->active_background < fc->max_background &&
	       !list_empty(&fc->bg_queue)) {
		struct fuse_req *req;

		req = list_entry(fc->bg_queue.next, struct fuse_req, list);
		list_del(&req->list);
		fc->active_background++;
		req->in.h.unique = fuse_get_unique(fc);
		queue_request(fc, req);
	}
}