static void end_polls(struct fuse_conn *fc)
{
	struct rb_node *p;

	p = rb_first(&fc->polled_files);

	while (p) {
		struct fuse_file *ff;
		ff = rb_entry(p, struct fuse_file, polled_node);
		wake_up_interruptible_all(&ff->poll_wait);

		p = rb_next(p);
	}
}