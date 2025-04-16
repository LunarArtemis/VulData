static int fuse_copy_fill(struct fuse_copy_state *cs)
{
	unsigned long offset;
	int err;

	unlock_request(cs->fc, cs->req);
	fuse_copy_finish(cs);
	if (cs->pipebufs) {
		struct pipe_buffer *buf = cs->pipebufs;

		if (!cs->write) {
			err = buf->ops->confirm(cs->pipe, buf);
			if (err)
				return err;

			BUG_ON(!cs->nr_segs);
			cs->currbuf = buf;
			cs->mapaddr = buf->ops->map(cs->pipe, buf, 0);
			cs->len = buf->len;
			cs->buf = cs->mapaddr + buf->offset;
			cs->pipebufs++;
			cs->nr_segs--;
		} else {
			struct page *page;

			if (cs->nr_segs == cs->pipe->buffers)
				return -EIO;

			page = alloc_page(GFP_HIGHUSER);
			if (!page)
				return -ENOMEM;

			buf->page = page;
			buf->offset = 0;
			buf->len = 0;

			cs->currbuf = buf;
			cs->mapaddr = kmap(page);
			cs->buf = cs->mapaddr;
			cs->len = PAGE_SIZE;
			cs->pipebufs++;
			cs->nr_segs++;
		}
	} else {
		if (!cs->seglen) {
			BUG_ON(!cs->nr_segs);
			cs->seglen = cs->iov[0].iov_len;
			cs->addr = (unsigned long) cs->iov[0].iov_base;
			cs->iov++;
			cs->nr_segs--;
		}
		err = get_user_pages_fast(cs->addr, 1, cs->write, &cs->pg);
		if (err < 0)
			return err;
		BUG_ON(err != 1);
		offset = cs->addr % PAGE_SIZE;
		cs->mapaddr = kmap(cs->pg);
		cs->buf = cs->mapaddr + offset;
		cs->len = min(PAGE_SIZE - offset, cs->seglen);
		cs->seglen -= cs->len;
		cs->addr += cs->len;
	}

	return lock_request(cs->fc, cs->req);
}