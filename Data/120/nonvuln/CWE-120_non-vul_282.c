static void wait_answer_interruptible(struct fuse_conn *fc,
				      struct fuse_req *req)
__releases(fc->lock)
__acquires(fc->lock)
{
	if (signal_pending(current))
		return;

	spin_unlock(&fc->lock);
	wait_event_interruptible(req->waitq, req->state == FUSE_REQ_FINISHED);
	spin_lock(&fc->lock);
}