static bfd_boolean
process_archive_index_and_symbols (struct archive_info *  arch,
				   unsigned int           sizeof_ar_index,
				   bfd_boolean            read_symbols)
{
  size_t got;
  unsigned long size;

  size = strtoul (arch->arhdr.ar_size, NULL, 10);
  /* PR 17531: file: 912bd7de.  */
  if ((signed long) size < 0)
    {
      error (_("%s: invalid archive header size: %ld\n"),
	     arch->file_name, size);
      return FALSE;
    }

  size = size + (size & 1);

  arch->next_arhdr_offset += sizeof arch->arhdr + size;

  if (! read_symbols)
    {
      if (fseek (arch->file, size, SEEK_CUR) != 0)
	{
	  error (_("%s: failed to skip archive symbol table\n"),
		 arch->file_name);
	  return FALSE;
	}
    }
  else
    {
      unsigned long i;
      /* A buffer used to hold numbers read in from an archive index.
	 These are always SIZEOF_AR_INDEX bytes long and stored in
	 big-endian format.  */
      unsigned char integer_buffer[sizeof arch->index_num];
      unsigned char * index_buffer;

      assert (sizeof_ar_index <= sizeof integer_buffer);

      /* Check the size of the archive index.  */
      if (size < sizeof_ar_index)
	{
	  error (_("%s: the archive index is empty\n"), arch->file_name);
	  return FALSE;
	}

      /* Read the number of entries in the archive index.  */
      got = fread (integer_buffer, 1, sizeof_ar_index, arch->file);
      if (got != sizeof_ar_index)
	{
	  error (_("%s: failed to read archive index\n"), arch->file_name);
	  return FALSE;
	}

      arch->index_num = byte_get_big_endian (integer_buffer, sizeof_ar_index);
      size -= sizeof_ar_index;

      if (size < arch->index_num * sizeof_ar_index
	  /* PR 17531: file: 585515d1.  */
	  || size < arch->index_num)
	{
	  error (_("%s: the archive index is supposed to have 0x%lx entries of %d bytes, but the size is only 0x%lx\n"),
		 arch->file_name, (long) arch->index_num, sizeof_ar_index, size);
	  return FALSE;
	}

      /* Read in the archive index.  */
      index_buffer = (unsigned char *)
	malloc (arch->index_num * sizeof_ar_index);
      if (index_buffer == NULL)
	{
	  error (_("Out of memory whilst trying to read archive symbol index\n"));
	  return FALSE;
	}

      got = fread (index_buffer, sizeof_ar_index, arch->index_num, arch->file);
      if (got != arch->index_num)
	{
	  free (index_buffer);
	  error (_("%s: failed to read archive index\n"), arch->file_name);
	  return FALSE;
	}

      size -= arch->index_num * sizeof_ar_index;

      /* Convert the index numbers into the host's numeric format.  */
      arch->index_array = (elf_vma *)
	malloc (arch->index_num * sizeof (* arch->index_array));
      if (arch->index_array == NULL)
	{
	  free (index_buffer);
	  error (_("Out of memory whilst trying to convert the archive symbol index\n"));
	  return FALSE;
	}

      for (i = 0; i < arch->index_num; i++)
	arch->index_array[i] =
	  byte_get_big_endian ((unsigned char *) (index_buffer + (i * sizeof_ar_index)),
			       sizeof_ar_index);
      free (index_buffer);

      /* The remaining space in the header is taken up by the symbol table.  */
      if (size < 1)
	{
	  error (_("%s: the archive has an index but no symbols\n"),
		 arch->file_name);
	  return FALSE;
	}

      arch->sym_table = (char *) malloc (size);
      if (arch->sym_table == NULL)
	{
	  error (_("Out of memory whilst trying to read archive index symbol table\n"));
	  return FALSE;
	}

      arch->sym_size = size;
      got = fread (arch->sym_table, 1, size, arch->file);
      if (got != size)
	{
	  error (_("%s: failed to read archive index symbol table\n"),
		 arch->file_name);
	  return FALSE;
	}
    }

  /* Read the next archive header.  */
  got = fread (&arch->arhdr, 1, sizeof arch->arhdr, arch->file);
  if (got != sizeof arch->arhdr && got != 0)
    {
      error (_("%s: failed to read archive header following archive index\n"),
	     arch->file_name);
      return FALSE;
    }

  return TRUE;
}