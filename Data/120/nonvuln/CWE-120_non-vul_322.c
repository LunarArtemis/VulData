static int forget_pending(struct fuse_conn *fc)
{
	return fc->forget_list_head.next != NULL;
}