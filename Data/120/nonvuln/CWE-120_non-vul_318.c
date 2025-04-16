void fuse_request_send(struct fuse_conn *fc, struct fuse_req *req)
{
	req->isreply = 1;
	spin_lock(&fc->lock);
	if (!fc->connected)
		req->out.h.error = -ENOTCONN;
	else if (fc->conn_error)
		req->out.h.error = -ECONNREFUSED;
	else {
		req->in.h.unique = fuse_get_unique(fc);
		queue_request(fc, req);
		/* acquire extra reference, since request is still needed
		   after request_end() */
		__fuse_get_request(req);

		request_wait_answer(fc, req);
	}
	spin_unlock(&fc->lock);
}