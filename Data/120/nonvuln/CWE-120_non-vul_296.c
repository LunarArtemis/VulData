static int fuse_notify_store(struct fuse_conn *fc, unsigned int size,
			     struct fuse_copy_state *cs)
{
	struct fuse_notify_store_out outarg;
	struct inode *inode;
	struct address_space *mapping;
	u64 nodeid;
	int err;
	pgoff_t index;
	unsigned int offset;
	unsigned int num;
	loff_t file_size;
	loff_t end;

	err = -EINVAL;
	if (size < sizeof(outarg))
		goto out_finish;

	err = fuse_copy_one(cs, &outarg, sizeof(outarg));
	if (err)
		goto out_finish;

	err = -EINVAL;
	if (size - sizeof(outarg) != outarg.size)
		goto out_finish;

	nodeid = outarg.nodeid;

	down_read(&fc->killsb);

	err = -ENOENT;
	if (!fc->sb)
		goto out_up_killsb;

	inode = ilookup5(fc->sb, nodeid, fuse_inode_eq, &nodeid);
	if (!inode)
		goto out_up_killsb;

	mapping = inode->i_mapping;
	index = outarg.offset >> PAGE_CACHE_SHIFT;
	offset = outarg.offset & ~PAGE_CACHE_MASK;
	file_size = i_size_read(inode);
	end = outarg.offset + outarg.size;
	if (end > file_size) {
		file_size = end;
		fuse_write_update_size(inode, file_size);
	}

	num = outarg.size;
	while (num) {
		struct page *page;
		unsigned int this_num;

		err = -ENOMEM;
		page = find_or_create_page(mapping, index,
					   mapping_gfp_mask(mapping));
		if (!page)
			goto out_iput;

		this_num = min_t(unsigned, num, PAGE_CACHE_SIZE - offset);
		err = fuse_copy_page(cs, &page, offset, this_num, 0);
		if (!err && offset == 0 && (num != 0 || file_size == end))
			SetPageUptodate(page);
		unlock_page(page);
		page_cache_release(page);

		if (err)
			goto out_iput;

		num -= this_num;
		offset = 0;
		index++;
	}

	err = 0;

out_iput:
	iput(inode);
out_up_killsb:
	up_read(&fc->killsb);
out_finish:
	fuse_copy_finish(cs);
	return err;
}