static int fuse_dev_pipe_buf_steal(struct pipe_inode_info *pipe,
				   struct pipe_buffer *buf)
{
	return 1;
}