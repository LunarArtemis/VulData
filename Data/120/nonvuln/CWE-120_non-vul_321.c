struct fuse_req *fuse_request_alloc(void)
{
	struct fuse_req *req = kmem_cache_alloc(fuse_req_cachep, GFP_KERNEL);
	if (req)
		fuse_request_init(req);
	return req;
}