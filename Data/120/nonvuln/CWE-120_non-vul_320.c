static void fuse_request_send_nowait(struct fuse_conn *fc, struct fuse_req *req)
{
	spin_lock(&fc->lock);
	if (fc->connected) {
		fuse_request_send_nowait_locked(fc, req);
		spin_unlock(&fc->lock);
	} else {
		req->out.h.error = -ENOTCONN;
		request_end(fc, req);
	}
}