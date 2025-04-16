static int fuse_notify_inval_inode(struct fuse_conn *fc, unsigned int size,
				   struct fuse_copy_state *cs)
{
	struct fuse_notify_inval_inode_out outarg;
	int err = -EINVAL;

	if (size != sizeof(outarg))
		goto err;

	err = fuse_copy_one(cs, &outarg, sizeof(outarg));
	if (err)
		goto err;
	fuse_copy_finish(cs);

	down_read(&fc->killsb);
	err = -ENOENT;
	if (fc->sb) {
		err = fuse_reverse_inval_inode(fc->sb, outarg.ino,
					       outarg.off, outarg.len);
	}
	up_read(&fc->killsb);
	return err;

err:
	fuse_copy_finish(cs);
	return err;
}