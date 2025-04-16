static int fuse_request_send_notify_reply(struct fuse_conn *fc,
					  struct fuse_req *req, u64 unique)
{
	int err = -ENODEV;

	req->isreply = 0;
	req->in.h.unique = unique;
	spin_lock(&fc->lock);
	if (fc->connected) {
		queue_request(fc, req);
		err = 0;
	}
	spin_unlock(&fc->lock);

	return err;
}