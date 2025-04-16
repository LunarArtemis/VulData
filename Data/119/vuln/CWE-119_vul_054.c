struct inode *nilfs_new_inode(struct inode *dir, umode_t mode)
{
	struct super_block *sb = dir->i_sb;
	struct the_nilfs *nilfs = sb->s_fs_info;
	struct inode *inode;
	struct nilfs_inode_info *ii;
	struct nilfs_root *root;
	int err = -ENOMEM;
	ino_t ino;

	inode = new_inode(sb);
	if (unlikely(!inode))
		goto failed;

	mapping_set_gfp_mask(inode->i_mapping,
			   mapping_gfp_constraint(inode->i_mapping, ~__GFP_FS));

	root = NILFS_I(dir)->i_root;
	ii = NILFS_I(inode);
	ii->i_state = BIT(NILFS_I_NEW);
	ii->i_root = root;

	err = nilfs_ifile_create_inode(root->ifile, &ino, &ii->i_bh);
	if (unlikely(err))
		goto failed_ifile_create_inode;
	/* reference count of i_bh inherits from nilfs_mdt_read_block() */

	atomic64_inc(&root->inodes_count);
	inode_init_owner(&init_user_ns, inode, dir, mode);
	inode->i_ino = ino;
	inode->i_mtime = inode->i_atime = inode->i_ctime = current_time(inode);

	if (S_ISREG(mode) || S_ISDIR(mode) || S_ISLNK(mode)) {
		err = nilfs_bmap_read(ii->i_bmap, NULL);
		if (err < 0)
			goto failed_after_creation;

		set_bit(NILFS_I_BMAP, &ii->i_state);
		/* No lock is needed; iget() ensures it. */
	}

	ii->i_flags = nilfs_mask_flags(
		mode, NILFS_I(dir)->i_flags & NILFS_FL_INHERITED);

	/* ii->i_file_acl = 0; */
	/* ii->i_dir_acl = 0; */
	ii->i_dir_start_lookup = 0;
	nilfs_set_inode_flags(inode);
	spin_lock(&nilfs->ns_next_gen_lock);
	inode->i_generation = nilfs->ns_next_generation++;
	spin_unlock(&nilfs->ns_next_gen_lock);
	if (nilfs_insert_inode_locked(inode, root, ino) < 0) {
		err = -EIO;
		goto failed_after_creation;
	}

	err = nilfs_init_acl(inode, dir);
	if (unlikely(err))
		/*
		 * Never occur.  When supporting nilfs_init_acl(),
		 * proper cancellation of above jobs should be considered.
		 */
		goto failed_after_creation;

	return inode;

 failed_after_creation:
	clear_nlink(inode);
	if (inode->i_state & I_NEW)
		unlock_new_inode(inode);
	iput(inode);  /*
		       * raw_inode will be deleted through
		       * nilfs_evict_inode().
		       */
	goto failed;

 failed_ifile_create_inode:
	make_bad_inode(inode);
	iput(inode);
 failed:
	return ERR_PTR(err);
}