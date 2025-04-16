zzip__new__ ZZIP_MEM_ENTRY *
zzip_mem_entry_new(ZZIP_DISK * disk, ZZIP_DISK_ENTRY * entry)
{
    if (! disk || ! entry)
        { errno=EINVAL; return 0; }
    ___ ZZIP_MEM_ENTRY *item = calloc(1, sizeof(*item));
    if (! item)
        return 0;               /* errno=ENOMEM; */
    ___ struct zzip_file_header *header =
        zzip_disk_entry_to_file_header(disk, entry);
    if (! header) 
    {
        debug1("no header in entry");
        free (item);
        return 0; /* errno=EBADMSG; */
    }
    /*  there is a number of duplicated information in the file header
     *  or the disk entry block. Theoretically some part may be missing
     *  that exists in the other, ... but we will prefer the disk entry.
     */
    item->zz_comment = zzip_disk_entry_strdup_comment(disk, entry);
    item->zz_name = zzip_disk_entry_strdup_name(disk, entry);
    item->zz_data = zzip_file_header_to_data(header);
    item->zz_flags = zzip_disk_entry_get_flags(entry);
    item->zz_compr = zzip_disk_entry_get_compr(entry);
    item->zz_mktime = zzip_disk_entry_get_mktime(entry);
    item->zz_crc32 = zzip_disk_entry_get_crc32(entry);
    item->zz_csize = zzip_disk_entry_get_csize(entry);
    item->zz_usize = zzip_disk_entry_get_usize(entry);
    item->zz_diskstart = zzip_disk_entry_get_diskstart(entry);
    item->zz_filetype = zzip_disk_entry_get_filetype(entry);

    /* zz_comment and zz_name are empty strings if not present on disk */
    if (! item->zz_comment || ! item->zz_name)
    {
        goto error; /* errno=ENOMEM */
    }

    {   /* copy the extra blocks to memory as well (maximum 64K each) */
        zzip_size_t /*    */ ext1_len = zzip_disk_entry_get_extras(entry);
        char *_zzip_restrict ext1_ptr = zzip_disk_entry_to_extras(entry);
        zzip_size_t /*    */ ext2_len = zzip_file_header_get_extras(header);
        char *_zzip_restrict ext2_ptr = zzip_file_header_to_extras(header);

        if (ext1_ptr + ext1_len >= disk->endbuf ||
            ext2_ptr + ext2_len >= disk->endbuf)
        {
            errno = EBADMSG; /* format error CVE-2017-5978 */
            goto error; /* zzip_mem_entry_free(item); return 0; */
        }

        if (ext1_len)
        {
            void *mem = malloc(ext1_len);
            if (! mem) goto error; /* errno = ENOMEM */
            item->zz_ext[1] = mem;
            item->zz_extlen[1] = ext1_len;
            memcpy(mem, ext1_ptr, ext1_len);
        }
        if (ext2_len)
        {
            void *mem = malloc(ext2_len);
            if (! mem) goto error; /* errno = ENOMEM */
            item->zz_ext[2] = mem;
            item->zz_extlen[2] = ext2_len;
            memcpy(mem, ext2_ptr, ext2_len);
        }
    }
    {
        /* override sizes/offsets with zip64 values for largefile support */
        zzip_extra_zip64 *block = (zzip_extra_zip64 *)
            zzip_mem_entry_find_extra_block(item, ZZIP_EXTRA_zip64, sizeof(zzip_extra_zip64));
        if (block)
        {
            item->zz_usize = ZZIP_GET64(block->z_usize);
            item->zz_csize = ZZIP_GET64(block->z_csize);
            item->zz_offset = ZZIP_GET64(block->z_offset);
            item->zz_diskstart = ZZIP_GET32(block->z_diskstart);
        }
    }
    /* NOTE:
     * All information from the central directory entry is now in memory.
     * Effectivly that allows us to modify it and write it back to disk.
     */
    return item;
    ____;
error:
    zzip_mem_entry_free(item);
    return 0;
    ____;
}