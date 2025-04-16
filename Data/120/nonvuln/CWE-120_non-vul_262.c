static void request_wait_answer(struct fuse_conn *fc, struct fuse_req *req)
__releases(fc->lock)
__acquires(fc->lock)
{
	if (!fc->no_interrupt) {
		/* Any signal may interrupt this */
		wait_answer_interruptible(fc, req);

		if (req->aborted)
			goto aborted;
		if (req->state == FUSE_REQ_FINISHED)
			return;

		req->interrupted = 1;
		if (req->state == FUSE_REQ_SENT)
			queue_interrupt(fc, req);
	}

	if (!req->force) {
		sigset_t oldset;

		/* Only fatal signals may interrupt this */
		block_sigs(&oldset);
		wait_answer_interruptible(fc, req);
		restore_sigs(&oldset);

		if (req->aborted)
			goto aborted;
		if (req->state == FUSE_REQ_FINISHED)
			return;

		/* Request is not yet in userspace, bail out */
		if (req->state == FUSE_REQ_PENDING) {
			list_del(&req->list);
			__fuse_put_request(req);
			req->out.h.error = -EINTR;
			return;
		}
	}

	/*
	 * Either request is already in userspace, or it was forced.
	 * Wait it out.
	 */
	spin_unlock(&fc->lock);
	wait_event(req->waitq, req->state == FUSE_REQ_FINISHED);
	spin_lock(&fc->lock);

	if (!req->aborted)
		return;

 aborted:
	BUG_ON(req->state != FUSE_REQ_FINISHED);
	if (req->locked) {
		/* This is uninterruptible sleep, because data is
		   being copied to/from the buffers of req.  During
		   locked state, there mustn't be any filesystem
		   operation (e.g. page fault), since that could lead
		   to deadlock */
		spin_unlock(&fc->lock);
		wait_event(req->waitq, !req->locked);
		spin_lock(&fc->lock);
	}
}