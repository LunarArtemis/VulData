static void queue_interrupt(struct fuse_conn *fc, struct fuse_req *req)
{
	list_add_tail(&req->intr_entry, &fc->interrupts);
	wake_up(&fc->waitq);
	kill_fasync(&fc->fasync, SIGIO, POLL_IN);
}