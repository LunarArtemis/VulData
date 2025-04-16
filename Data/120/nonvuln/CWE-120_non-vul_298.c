static int fuse_read_batch_forget(struct fuse_conn *fc,
				   struct fuse_copy_state *cs, size_t nbytes)
__releases(fc->lock)
{
	int err;
	unsigned max_forgets;
	unsigned count;
	struct fuse_forget_link *head;
	struct fuse_batch_forget_in arg = { .count = 0 };
	struct fuse_in_header ih = {
		.opcode = FUSE_BATCH_FORGET,
		.unique = fuse_get_unique(fc),
		.len = sizeof(ih) + sizeof(arg),
	};

	if (nbytes < ih.len) {
		spin_unlock(&fc->lock);
		return -EINVAL;
	}

	max_forgets = (nbytes - ih.len) / sizeof(struct fuse_forget_one);
	head = dequeue_forget(fc, max_forgets, &count);
	spin_unlock(&fc->lock);

	arg.count = count;
	ih.len += count * sizeof(struct fuse_forget_one);
	err = fuse_copy_one(cs, &ih, sizeof(ih));
	if (!err)
		err = fuse_copy_one(cs, &arg, sizeof(arg));

	while (head) {
		struct fuse_forget_link *forget = head;

		if (!err) {
			err = fuse_copy_one(cs, &forget->forget_one,
					    sizeof(forget->forget_one));
		}
		head = forget->next;
		kfree(forget);
	}

	fuse_copy_finish(cs);

	if (err)
		return err;

	return ih.len;
}