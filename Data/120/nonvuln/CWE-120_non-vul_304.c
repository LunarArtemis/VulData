void fuse_queue_forget(struct fuse_conn *fc, struct fuse_forget_link *forget,
		       u64 nodeid, u64 nlookup)
{
	forget->forget_one.nodeid = nodeid;
	forget->forget_one.nlookup = nlookup;

	spin_lock(&fc->lock);
	fc->forget_list_tail->next = forget;
	fc->forget_list_tail = forget;
	wake_up(&fc->waitq);
	kill_fasync(&fc->fasync, SIGIO, POLL_IN);
	spin_unlock(&fc->lock);
}