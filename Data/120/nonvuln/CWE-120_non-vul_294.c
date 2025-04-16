static void __fuse_get_request(struct fuse_req *req)
{
	atomic_inc(&req->count);
}