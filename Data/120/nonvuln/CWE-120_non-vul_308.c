static void fuse_copy_finish(struct fuse_copy_state *cs)
{
	if (cs->currbuf) {
		struct pipe_buffer *buf = cs->currbuf;

		if (!cs->write) {
			buf->ops->unmap(cs->pipe, buf, cs->mapaddr);
		} else {
			kunmap(buf->page);
			buf->len = PAGE_SIZE - cs->len;
		}
		cs->currbuf = NULL;
		cs->mapaddr = NULL;
	} else if (cs->mapaddr) {
		kunmap(cs->pg);
		if (cs->write) {
			flush_dcache_page(cs->pg);
			set_page_dirty_lock(cs->pg);
		}
		put_page(cs->pg);
		cs->mapaddr = NULL;
	}
}