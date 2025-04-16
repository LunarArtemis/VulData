static void fuse_copy_init(struct fuse_copy_state *cs, struct fuse_conn *fc,
			   int write,
			   const struct iovec *iov, unsigned long nr_segs)
{
	memset(cs, 0, sizeof(*cs));
	cs->fc = fc;
	cs->write = write;
	cs->iov = iov;
	cs->nr_segs = nr_segs;
}