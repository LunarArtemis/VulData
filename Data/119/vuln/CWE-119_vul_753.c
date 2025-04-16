static int udf_symlink_filler(struct file *file, struct page *page)
{
	struct inode *inode = page->mapping->host;
	struct buffer_head *bh = NULL;
	unsigned char *symlink;
	int err = -EIO;
	unsigned char *p = kmap(page);
	struct udf_inode_info *iinfo;
	uint32_t pos;

	iinfo = UDF_I(inode);
	pos = udf_block_map(inode, 0);

	down_read(&iinfo->i_data_sem);
	if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_IN_ICB) {
		symlink = iinfo->i_ext.i_data + iinfo->i_lenEAttr;
	} else {
		bh = sb_bread(inode->i_sb, pos);

		if (!bh)
			goto out;

		symlink = bh->b_data;
	}

	udf_pc_to_char(inode->i_sb, symlink, inode->i_size, p);
	brelse(bh);

	up_read(&iinfo->i_data_sem);
	SetPageUptodate(page);
	kunmap(page);
	unlock_page(page);
	return 0;

out:
	up_read(&iinfo->i_data_sem);
	SetPageError(page);
	kunmap(page);
	unlock_page(page);
	return err;
}