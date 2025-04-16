void fuse_abort_conn(struct fuse_conn *fc)
{
	spin_lock(&fc->lock);
	if (fc->connected) {
		fc->connected = 0;
		fc->blocked = 0;
		end_io_requests(fc);
		end_queued_requests(fc);
		end_polls(fc);
		wake_up_all(&fc->waitq);
		wake_up_all(&fc->blocked_waitq);
		kill_fasync(&fc->fasync, SIGIO, POLL_IN);
	}
	spin_unlock(&fc->lock);
}