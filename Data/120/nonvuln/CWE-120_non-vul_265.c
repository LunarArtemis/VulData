static ssize_t fuse_dev_write(struct kiocb *iocb, const struct iovec *iov,
			      unsigned long nr_segs, loff_t pos)
{
	struct fuse_copy_state cs;
	struct fuse_conn *fc = fuse_get_conn(iocb->ki_filp);
	if (!fc)
		return -EPERM;

	fuse_copy_init(&cs, fc, 0, iov, nr_segs);

	return fuse_dev_do_write(fc, &cs, iov_length(iov, nr_segs));
}