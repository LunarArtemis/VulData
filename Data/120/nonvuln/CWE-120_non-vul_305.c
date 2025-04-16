struct fuse_req *fuse_request_alloc_nofs(void)
{
	struct fuse_req *req = kmem_cache_alloc(fuse_req_cachep, GFP_NOFS);
	if (req)
		fuse_request_init(req);
	return req;
}