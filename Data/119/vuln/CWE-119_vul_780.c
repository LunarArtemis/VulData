const char *
_bfd_coff_read_string_table (bfd *abfd)
{
  char extstrsize[STRING_SIZE_SIZE];
  bfd_size_type strsize;
  char *strings;
  file_ptr pos;

  if (obj_coff_strings (abfd) != NULL)
    return obj_coff_strings (abfd);

  if (obj_sym_filepos (abfd) == 0)
    {
      bfd_set_error (bfd_error_no_symbols);
      return NULL;
    }

  pos = obj_sym_filepos (abfd);
  pos += obj_raw_syment_count (abfd) * bfd_coff_symesz (abfd);
  if (bfd_seek (abfd, pos, SEEK_SET) != 0)
    return NULL;

  if (bfd_bread (extstrsize, (bfd_size_type) sizeof extstrsize, abfd)
      != sizeof extstrsize)
    {
      if (bfd_get_error () != bfd_error_file_truncated)
	return NULL;

      /* There is no string table.  */
      strsize = STRING_SIZE_SIZE;
    }
  else
    {
#if STRING_SIZE_SIZE == 4
      strsize = H_GET_32 (abfd, extstrsize);
#else
 #error Change H_GET_32
#endif
    }

  if (strsize < STRING_SIZE_SIZE)
    {
      _bfd_error_handler
	/* xgettext: c-format */
	(_("%B: bad string table size %Lu"), abfd, strsize);
      bfd_set_error (bfd_error_bad_value);
      return NULL;
    }

  strings = (char *) bfd_malloc (strsize + 1);
  if (strings == NULL)
    return NULL;

  /* PR 17521 file: 079-54929-0.004.
     A corrupt file could contain an index that points into the first
     STRING_SIZE_SIZE bytes of the string table, so make sure that
     they are zero.  */
  memset (strings, 0, STRING_SIZE_SIZE);

  if (bfd_bread (strings + STRING_SIZE_SIZE, strsize - STRING_SIZE_SIZE, abfd)
      != strsize - STRING_SIZE_SIZE)
    {
      free (strings);
      return NULL;
    }

  obj_coff_strings (abfd) = strings;
  obj_coff_strings_len (abfd) = strsize;
  /* Terminate the string table, just in case.  */
  strings[strsize] = 0;
  return strings;
}