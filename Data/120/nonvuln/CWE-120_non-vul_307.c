static int fuse_read_single_forget(struct fuse_conn *fc,
				   struct fuse_copy_state *cs,
				   size_t nbytes)
__releases(fc->lock)
{
	int err;
	struct fuse_forget_link *forget = dequeue_forget(fc, 1, NULL);
	struct fuse_forget_in arg = {
		.nlookup = forget->forget_one.nlookup,
	};
	struct fuse_in_header ih = {
		.opcode = FUSE_FORGET,
		.nodeid = forget->forget_one.nodeid,
		.unique = fuse_get_unique(fc),
		.len = sizeof(ih) + sizeof(arg),
	};

	spin_unlock(&fc->lock);
	kfree(forget);
	if (nbytes < ih.len)
		return -EINVAL;

	err = fuse_copy_one(cs, &ih, sizeof(ih));
	if (!err)
		err = fuse_copy_one(cs, &arg, sizeof(arg));
	fuse_copy_finish(cs);

	if (err)
		return err;

	return ih.len;
}