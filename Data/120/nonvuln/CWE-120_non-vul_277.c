static void fuse_req_init_context(struct fuse_req *req)
{
	req->in.h.uid = current_fsuid();
	req->in.h.gid = current_fsgid();
	req->in.h.pid = current->pid;
}