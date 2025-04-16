static int fuse_retrieve(struct fuse_conn *fc, struct inode *inode,
			 struct fuse_notify_retrieve_out *outarg)
{
	int err;
	struct address_space *mapping = inode->i_mapping;
	struct fuse_req *req;
	pgoff_t index;
	loff_t file_size;
	unsigned int num;
	unsigned int offset;
	size_t total_len = 0;

	req = fuse_get_req(fc);
	if (IS_ERR(req))
		return PTR_ERR(req);

	offset = outarg->offset & ~PAGE_CACHE_MASK;

	req->in.h.opcode = FUSE_NOTIFY_REPLY;
	req->in.h.nodeid = outarg->nodeid;
	req->in.numargs = 2;
	req->in.argpages = 1;
	req->page_offset = offset;
	req->end = fuse_retrieve_end;

	index = outarg->offset >> PAGE_CACHE_SHIFT;
	file_size = i_size_read(inode);
	num = outarg->size;
	if (outarg->offset > file_size)
		num = 0;
	else if (outarg->offset + num > file_size)
		num = file_size - outarg->offset;

	while (num) {
		struct page *page;
		unsigned int this_num;

		page = find_get_page(mapping, index);
		if (!page)
			break;

		this_num = min_t(unsigned, num, PAGE_CACHE_SIZE - offset);
		req->pages[req->num_pages] = page;
		req->num_pages++;

		num -= this_num;
		total_len += this_num;
	}
	req->misc.retrieve_in.offset = outarg->offset;
	req->misc.retrieve_in.size = total_len;
	req->in.args[0].size = sizeof(req->misc.retrieve_in);
	req->in.args[0].value = &req->misc.retrieve_in;
	req->in.args[1].size = total_len;

	err = fuse_request_send_notify_reply(fc, req, outarg->notify_unique);
	if (err)
		fuse_retrieve_end(fc, req);

	return err;
}