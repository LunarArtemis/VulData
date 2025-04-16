struct fuse_req *fuse_get_req_nofail(struct fuse_conn *fc, struct file *file)
{
	struct fuse_req *req;

	atomic_inc(&fc->num_waiting);
	wait_event(fc->blocked_waitq, !fc->blocked);
	req = fuse_request_alloc();
	if (!req)
		req = get_reserved_req(fc, file);

	fuse_req_init_context(req);
	req->waiting = 1;
	return req;
}