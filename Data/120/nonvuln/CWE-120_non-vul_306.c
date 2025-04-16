void fuse_request_send_background(struct fuse_conn *fc, struct fuse_req *req)
{
	req->isreply = 1;
	fuse_request_send_nowait(fc, req);
}