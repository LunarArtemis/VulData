static int fuse_try_move_page(struct fuse_copy_state *cs, struct page **pagep)
{
	int err;
	struct page *oldpage = *pagep;
	struct page *newpage;
	struct pipe_buffer *buf = cs->pipebufs;
	struct address_space *mapping;
	pgoff_t index;

	unlock_request(cs->fc, cs->req);
	fuse_copy_finish(cs);

	err = buf->ops->confirm(cs->pipe, buf);
	if (err)
		return err;

	BUG_ON(!cs->nr_segs);
	cs->currbuf = buf;
	cs->len = buf->len;
	cs->pipebufs++;
	cs->nr_segs--;

	if (cs->len != PAGE_SIZE)
		goto out_fallback;

	if (buf->ops->steal(cs->pipe, buf) != 0)
		goto out_fallback;

	newpage = buf->page;

	if (WARN_ON(!PageUptodate(newpage)))
		return -EIO;

	ClearPageMappedToDisk(newpage);

	if (fuse_check_page(newpage) != 0)
		goto out_fallback_unlock;

	mapping = oldpage->mapping;
	index = oldpage->index;

	/*
	 * This is a new and locked page, it shouldn't be mapped or
	 * have any special flags on it
	 */
	if (WARN_ON(page_mapped(oldpage)))
		goto out_fallback_unlock;
	if (WARN_ON(page_has_private(oldpage)))
		goto out_fallback_unlock;
	if (WARN_ON(PageDirty(oldpage) || PageWriteback(oldpage)))
		goto out_fallback_unlock;
	if (WARN_ON(PageMlocked(oldpage)))
		goto out_fallback_unlock;

	err = replace_page_cache_page(oldpage, newpage, GFP_KERNEL);
	if (err) {
		unlock_page(newpage);
		return err;
	}

	page_cache_get(newpage);

	if (!(buf->flags & PIPE_BUF_FLAG_LRU))
		lru_cache_add_file(newpage);

	err = 0;
	spin_lock(&cs->fc->lock);
	if (cs->req->aborted)
		err = -ENOENT;
	else
		*pagep = newpage;
	spin_unlock(&cs->fc->lock);

	if (err) {
		unlock_page(newpage);
		page_cache_release(newpage);
		return err;
	}

	unlock_page(oldpage);
	page_cache_release(oldpage);
	cs->len = 0;

	return 0;

out_fallback_unlock:
	unlock_page(newpage);
out_fallback:
	cs->mapaddr = buf->ops->map(cs->pipe, buf, 1);
	cs->buf = cs->mapaddr + buf->offset;

	err = lock_request(cs->fc, cs->req);
	if (err)
		return err;

	return 1;
}