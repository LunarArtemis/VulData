int
setup_archive (struct archive_info *arch, const char *file_name,
	       FILE *file, bfd_boolean is_thin_archive,
	       bfd_boolean read_symbols)
{
  size_t got;

  arch->file_name = strdup (file_name);
  arch->file = file;
  arch->index_num = 0;
  arch->index_array = NULL;
  arch->sym_table = NULL;
  arch->sym_size = 0;
  arch->longnames = NULL;
  arch->longnames_size = 0;
  arch->nested_member_origin = 0;
  arch->is_thin_archive = is_thin_archive;
  arch->uses_64bit_indicies = FALSE;
  arch->next_arhdr_offset = SARMAG;

  /* Read the first archive member header.  */
  if (fseek (file, SARMAG, SEEK_SET) != 0)
    {
      error (_("%s: failed to seek to first archive header\n"), file_name);
      return 1;
    }
  got = fread (&arch->arhdr, 1, sizeof arch->arhdr, file);
  if (got != sizeof arch->arhdr)
    {
      if (got == 0)
	return 0;

      error (_("%s: failed to read archive header\n"), file_name);
      return 1;
    }

  /* See if this is the archive symbol table.  */
  if (const_strneq (arch->arhdr.ar_name, "/               "))
    {
      if (! process_archive_index_and_symbols (arch, 4, read_symbols))
	return 1;
    }
  else if (const_strneq (arch->arhdr.ar_name, "/SYM64/         "))
    {
      arch->uses_64bit_indicies = TRUE;
      if (! process_archive_index_and_symbols (arch, 8, read_symbols))
	return 1;
    }
  else if (read_symbols)
    printf (_("%s has no archive index\n"), file_name);

  if (const_strneq (arch->arhdr.ar_name, "//              "))
    {
      /* This is the archive string table holding long member names.  */
      arch->longnames_size = strtoul (arch->arhdr.ar_size, NULL, 10);
      /* PR 17531: file: 01068045.  */
      if (arch->longnames_size < 8)
	{
	  error (_("%s: long name table is too small, (size = %ld)\n"),
		 file_name, arch->longnames_size);
	  return 1;
	}
      /* PR 17531: file: 639d6a26.  */
      if ((signed long) arch->longnames_size < 0)
	{
	  error (_("%s: long name table is too big, (size = 0x%lx)\n"),
		 file_name, arch->longnames_size);
	  return 1;
	}

      arch->next_arhdr_offset += sizeof arch->arhdr + arch->longnames_size;

      /* Plus one to allow for a string terminator.  */
      arch->longnames = (char *) malloc (arch->longnames_size + 1);
      if (arch->longnames == NULL)
	{
	  error (_("Out of memory reading long symbol names in archive\n"));
	  return 1;
	}

      if (fread (arch->longnames, arch->longnames_size, 1, file) != 1)
	{
	  free (arch->longnames);
	  arch->longnames = NULL;
	  error (_("%s: failed to read long symbol name string table\n"),
		 file_name);
	  return 1;
	}

      if ((arch->longnames_size & 1) != 0)
	getc (file);

      arch->longnames[arch->longnames_size] = 0;
    }

  return 0;
}