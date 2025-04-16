static void end_queued_requests(struct fuse_conn *fc)
__releases(fc->lock)
__acquires(fc->lock)
{
	fc->max_background = UINT_MAX;
	flush_bg_queue(fc);
	end_requests(fc, &fc->pending);
	end_requests(fc, &fc->processing);
	while (forget_pending(fc))
		kfree(dequeue_forget(fc, 1, NULL));
}