static int fuse_ref_page(struct fuse_copy_state *cs, struct page *page,
			 unsigned offset, unsigned count)
{
	struct pipe_buffer *buf;

	if (cs->nr_segs == cs->pipe->buffers)
		return -EIO;

	unlock_request(cs->fc, cs->req);
	fuse_copy_finish(cs);

	buf = cs->pipebufs;
	page_cache_get(page);
	buf->page = page;
	buf->offset = offset;
	buf->len = count;

	cs->pipebufs++;
	cs->nr_segs++;
	cs->len = 0;

	return 0;
}