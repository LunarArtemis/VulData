static int fuse_notify_retrieve(struct fuse_conn *fc, unsigned int size,
				struct fuse_copy_state *cs)
{
	struct fuse_notify_retrieve_out outarg;
	struct inode *inode;
	int err;

	err = -EINVAL;
	if (size != sizeof(outarg))
		goto copy_finish;

	err = fuse_copy_one(cs, &outarg, sizeof(outarg));
	if (err)
		goto copy_finish;

	fuse_copy_finish(cs);

	down_read(&fc->killsb);
	err = -ENOENT;
	if (fc->sb) {
		u64 nodeid = outarg.nodeid;

		inode = ilookup5(fc->sb, nodeid, fuse_inode_eq, &nodeid);
		if (inode) {
			err = fuse_retrieve(fc, inode, &outarg);
			iput(inode);
		}
	}
	up_read(&fc->killsb);

	return err;

copy_finish:
	fuse_copy_finish(cs);
	return err;
}