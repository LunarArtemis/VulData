static ssize_t fuse_dev_read(struct kiocb *iocb, const struct iovec *iov,
			      unsigned long nr_segs, loff_t pos)
{
	struct fuse_copy_state cs;
	struct file *file = iocb->ki_filp;
	struct fuse_conn *fc = fuse_get_conn(file);
	if (!fc)
		return -EPERM;

	fuse_copy_init(&cs, fc, 1, iov, nr_segs);

	return fuse_dev_do_read(fc, file, &cs, iov_length(iov, nr_segs));
}