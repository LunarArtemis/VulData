static bfd_boolean
coff_slurp_line_table (bfd *abfd, asection *asect)
{
  LINENO *native_lineno;
  alent *lineno_cache;
  bfd_size_type amt;
  unsigned int counter;
  alent *cache_ptr;
  bfd_vma prev_offset = 0;
  bfd_boolean ordered = TRUE;
  unsigned int nbr_func;
  LINENO *src;
  bfd_boolean have_func;
  bfd_boolean ret = TRUE;

  BFD_ASSERT (asect->lineno == NULL);

  amt = ((bfd_size_type) asect->lineno_count + 1) * sizeof (alent);
  lineno_cache = (alent *) bfd_alloc (abfd, amt);
  if (lineno_cache == NULL)
    return FALSE;

  amt = (bfd_size_type) bfd_coff_linesz (abfd) * asect->lineno_count;
  native_lineno = (LINENO *) buy_and_read (abfd, asect->line_filepos, amt);
  if (native_lineno == NULL)
    {
      _bfd_error_handler
	(_("%B: warning: line number table read failed"), abfd);
      bfd_release (abfd, lineno_cache);
      return FALSE;
    }

  cache_ptr = lineno_cache;
  asect->lineno = lineno_cache;
  src = native_lineno;
  nbr_func = 0;
  have_func = FALSE;

  for (counter = 0; counter < asect->lineno_count; counter++, src++)
    {
      struct internal_lineno dst;

      bfd_coff_swap_lineno_in (abfd, src, &dst);
      cache_ptr->line_number = dst.l_lnno;
      /* Appease memory checkers that get all excited about
	 uninitialised memory when copying alents if u.offset is
	 larger than u.sym.  (64-bit BFD on 32-bit host.)  */
      memset (&cache_ptr->u, 0, sizeof (cache_ptr->u));

      if (cache_ptr->line_number == 0)
	{
	  combined_entry_type * ent;
	  unsigned long symndx;
	  coff_symbol_type *sym;

	  have_func = FALSE;
	  symndx = dst.l_addr.l_symndx;
	  if (symndx >= obj_raw_syment_count (abfd))
	    {
	      _bfd_error_handler
		/* xgettext:c-format */
		(_("%B: warning: illegal symbol index 0x%lx in line number entry %d"),
		 abfd, symndx, counter);
	      cache_ptr->line_number = -1;
	      ret = FALSE;
	      continue;
	    }

	  ent = obj_raw_syments (abfd) + symndx;
	  /* FIXME: We should not be casting between ints and
	     pointers like this.  */
	  if (! ent->is_sym)
	    {
	      _bfd_error_handler
		/* xgettext:c-format */
		(_("%B: warning: illegal symbol index 0x%lx in line number entry %d"),
		 abfd, symndx, counter);
	      cache_ptr->line_number = -1;
	      ret = FALSE;
	      continue;
	    }
	  sym = (coff_symbol_type *) (ent->u.syment._n._n_n._n_zeroes);

	  /* PR 17512 file: 078-10659-0.004  */
	  if (sym < obj_symbols (abfd)
	      || sym >= obj_symbols (abfd) + bfd_get_symcount (abfd))
	    {
	      _bfd_error_handler
		/* xgettext:c-format */
		(_("%B: warning: illegal symbol in line number entry %d"),
		 abfd, counter);
	      cache_ptr->line_number = -1;
	      ret = FALSE;
	      continue;
	    }

	  have_func = TRUE;
	  nbr_func++;
	  cache_ptr->u.sym = (asymbol *) sym;
	  if (sym->lineno != NULL)
	    _bfd_error_handler
	      /* xgettext:c-format */
	      (_("%B: warning: duplicate line number information for `%s'"),
	       abfd, bfd_asymbol_name (&sym->symbol));

	  sym->lineno = cache_ptr;
	  if (sym->symbol.value < prev_offset)
	    ordered = FALSE;
	  prev_offset = sym->symbol.value;
	}
      else if (!have_func)
	/* Drop line information that has no associated function.
	   PR 17521: file: 078-10659-0.004.  */
	continue;
      else
	cache_ptr->u.offset = (dst.l_addr.l_paddr
			       - bfd_section_vma (abfd, asect));
      cache_ptr++;
    }

  asect->lineno_count = cache_ptr - lineno_cache;
  memset (cache_ptr, 0, sizeof (*cache_ptr));
  bfd_release (abfd, native_lineno);

  /* On some systems (eg AIX5.3) the lineno table may not be sorted.  */
  if (!ordered)
    {
      /* Sort the table.  */
      alent **func_table;
      alent *n_lineno_cache;

      /* Create a table of functions.  */
      func_table = (alent **) bfd_alloc (abfd, nbr_func * sizeof (alent *));
      if (func_table != NULL)
	{
	  alent **p = func_table;
	  unsigned int i;

	  for (i = 0; i < asect->lineno_count; i++)
	    if (lineno_cache[i].line_number == 0)
	      *p++ = &lineno_cache[i];

	  BFD_ASSERT ((unsigned int) (p - func_table) == nbr_func);

	  /* Sort by functions.  */
	  qsort (func_table, nbr_func, sizeof (alent *), coff_sort_func_alent);

	  /* Create the new sorted table.  */
	  amt = (bfd_size_type) asect->lineno_count * sizeof (alent);
	  n_lineno_cache = (alent *) bfd_alloc (abfd, amt);
	  if (n_lineno_cache != NULL)
	    {
	      alent *n_cache_ptr = n_lineno_cache;

	      for (i = 0; i < nbr_func; i++)
		{
		  coff_symbol_type *sym;
		  alent *old_ptr = func_table[i];

		  /* Update the function entry.  */
		  sym = (coff_symbol_type *) old_ptr->u.sym;
		  /* PR binutils/17512: Point the lineno to where
		     this entry will be after the memcpy below.  */
		  sym->lineno = lineno_cache + (n_cache_ptr - n_lineno_cache);
		  /* Copy the function and line number entries.  */
		  do
		    *n_cache_ptr++ = *old_ptr++;
		  while (old_ptr->line_number != 0);
		}
	      BFD_ASSERT ((bfd_size_type) (n_cache_ptr - n_lineno_cache) == (amt / sizeof (alent)));

	      memcpy (lineno_cache, n_lineno_cache, amt);
	    }
	  else
	    ret = FALSE;
	  bfd_release (abfd, func_table);
	}
      else
	ret = FALSE;
    }

  return ret;
}