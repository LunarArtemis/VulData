static bfd_boolean
aout_get_external_symbols (bfd *abfd)
{
  if (obj_aout_external_syms (abfd) == NULL)
    {
      bfd_size_type count;
      struct external_nlist *syms;
      bfd_size_type amt = exec_hdr (abfd)->a_syms;

      count = amt / EXTERNAL_NLIST_SIZE;
      if (count == 0)
	return TRUE;		/* Nothing to do.  */

#ifdef USE_MMAP
      if (! bfd_get_file_window (abfd, obj_sym_filepos (abfd), amt,
				 &obj_aout_sym_window (abfd), TRUE))
	return FALSE;
      syms = (struct external_nlist *) obj_aout_sym_window (abfd).data;
#else
      /* We allocate using malloc to make the values easy to free
	 later on.  If we put them on the objalloc it might not be
	 possible to free them.  */
      syms = (struct external_nlist *) bfd_malloc (amt);
      if (syms == NULL)
	return FALSE;

      if (bfd_seek (abfd, obj_sym_filepos (abfd), SEEK_SET) != 0
	  || bfd_bread (syms, amt, abfd) != amt)
	{
	  free (syms);
	  return FALSE;
	}
#endif

      obj_aout_external_syms (abfd) = syms;
      obj_aout_external_sym_count (abfd) = count;
    }

  if (obj_aout_external_strings (abfd) == NULL
      && exec_hdr (abfd)->a_syms != 0)
    {
      unsigned char string_chars[BYTES_IN_WORD];
      bfd_size_type stringsize;
      char *strings;
      bfd_size_type amt = BYTES_IN_WORD;

      /* Get the size of the strings.  */
      if (bfd_seek (abfd, obj_str_filepos (abfd), SEEK_SET) != 0
	  || bfd_bread ((void *) string_chars, amt, abfd) != amt)
	return FALSE;
      stringsize = GET_WORD (abfd, string_chars);

#ifdef USE_MMAP
      if (! bfd_get_file_window (abfd, obj_str_filepos (abfd), stringsize,
				 &obj_aout_string_window (abfd), TRUE))
	return FALSE;
      strings = (char *) obj_aout_string_window (abfd).data;
#else
      strings = (char *) bfd_malloc (stringsize + 1);
      if (strings == NULL)
	return FALSE;

      /* Skip space for the string count in the buffer for convenience
	 when using indexes.  */
      amt = stringsize - BYTES_IN_WORD;
      if (bfd_bread (strings + BYTES_IN_WORD, amt, abfd) != amt)
	{
	  free (strings);
	  return FALSE;
	}
#endif

      /* Ensure that a zero index yields an empty string.  */
      strings[0] = '\0';

      strings[stringsize - 1] = 0;

      obj_aout_external_strings (abfd) = strings;
      obj_aout_external_string_size (abfd) = stringsize;
    }

  return TRUE;
}