static int fuse_read_forget(struct fuse_conn *fc, struct fuse_copy_state *cs,
			    size_t nbytes)
__releases(fc->lock)
{
	if (fc->minor < 16 || fc->forget_list_head.next->next == NULL)
		return fuse_read_single_forget(fc, cs, nbytes);
	else
		return fuse_read_batch_forget(fc, cs, nbytes);
}