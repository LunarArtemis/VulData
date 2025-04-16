static long
elf_x86_64_get_synthetic_symtab (bfd *abfd,
				 long symcount ATTRIBUTE_UNUSED,
				 asymbol **syms ATTRIBUTE_UNUSED,
				 long dynsymcount,
				 asymbol **dynsyms,
				 asymbol **ret)
{
  long size, count, i, n;
  int j;
  unsigned int plt_got_offset, plt_entry_size, plt_got_insn_size;
  asymbol *s;
  bfd_byte *plt_contents;
  long dynrelcount, relsize;
  arelent **dynrelbuf;
  const struct elf_x86_64_lazy_plt_layout *lazy_plt;
  const struct elf_x86_64_non_lazy_plt_layout *non_lazy_plt;
  const struct elf_x86_64_lazy_plt_layout *lazy_bnd_plt;
  const struct elf_x86_64_non_lazy_plt_layout *non_lazy_bnd_plt;
  const struct elf_x86_64_lazy_plt_layout *lazy_ibt_plt;
  const struct elf_x86_64_non_lazy_plt_layout *non_lazy_ibt_plt;
  asection *plt;
  char *names;
  enum elf_x86_64_plt_type plt_type;
  struct elf_x86_64_plt plts[] =
    {
      { ".plt", NULL, NULL, plt_unknown, 0, 0, 0, 0 },
      { ".plt.got", NULL, NULL, plt_non_lazy, 0, 0, 0, 0 },
      { ".plt.sec", NULL, NULL, plt_second, 0, 0, 0, 0 },
      { ".plt.bnd", NULL, NULL, plt_second, 0, 0, 0, 0 },
      { NULL, NULL, NULL, plt_non_lazy, 0, 0, 0, 0 }
    };

  *ret = NULL;

  if ((abfd->flags & (DYNAMIC | EXEC_P)) == 0)
    return 0;

  if (dynsymcount <= 0)
    return 0;

  relsize = bfd_get_dynamic_reloc_upper_bound (abfd);
  if (relsize <= 0)
    return -1;

  dynrelbuf = (arelent **) bfd_malloc (relsize);
  if (dynrelbuf == NULL)
    return -1;

  dynrelcount = bfd_canonicalize_dynamic_reloc (abfd, dynrelbuf,
						dynsyms);
  if (dynrelcount <= 0)
    return -1;

  /* Sort the relocs by address.  */
  qsort (dynrelbuf, dynrelcount, sizeof (arelent *), compare_relocs);

  if (get_elf_x86_64_backend_data (abfd)->os == is_normal)
    {
      lazy_plt = &elf_x86_64_lazy_plt;
      non_lazy_plt = &elf_x86_64_non_lazy_plt;
      lazy_bnd_plt = &elf_x86_64_lazy_bnd_plt;
      non_lazy_bnd_plt = &elf_x86_64_non_lazy_bnd_plt;
      if (ABI_64_P (abfd))
	{
	  lazy_ibt_plt = &elf_x86_64_lazy_ibt_plt;
	  non_lazy_ibt_plt = &elf_x86_64_non_lazy_ibt_plt;
	}
      else
	{
	  lazy_ibt_plt = &elf_x32_lazy_ibt_plt;
	  non_lazy_ibt_plt = &elf_x32_non_lazy_ibt_plt;
	}
    }
  else
    {
      lazy_plt = &elf_x86_64_nacl_plt;
      non_lazy_plt = NULL;
      lazy_bnd_plt = NULL;
      non_lazy_bnd_plt = NULL;
      lazy_ibt_plt = NULL;
      non_lazy_ibt_plt = NULL;
    }

  count = 0;
  for (j = 0; plts[j].name != NULL; j++)
    {
      plt = bfd_get_section_by_name (abfd, plts[j].name);
      if (plt == NULL || plt->size == 0)
	continue;

      /* Get the PLT section contents.  */
      plt_contents = (bfd_byte *) bfd_malloc (plt->size);
      if (plt_contents == NULL)
	break;
      if (!bfd_get_section_contents (abfd, (asection *) plt,
				     plt_contents, 0, plt->size))
	{
	  free (plt_contents);
	  break;
	}

      /* Check what kind of PLT it is.  */
      plt_type = plt_unknown;
      if (plts[j].type == plt_unknown
	  && (plt->size >= (lazy_plt->plt_entry_size
			    + lazy_plt->plt_entry_size)))
	{
	  /* Match lazy PLT first.  Need to check the first two
	     instructions.   */
	  if ((memcmp (plt_contents, lazy_plt->plt0_entry,
		       lazy_plt->plt0_got1_offset) == 0)
	      && (memcmp (plt_contents + 6, lazy_plt->plt0_entry + 6,
			  2) == 0))
	    plt_type = plt_lazy;
	  else if (lazy_bnd_plt != NULL
		   && (memcmp (plt_contents, lazy_bnd_plt->plt0_entry,
			       lazy_bnd_plt->plt0_got1_offset) == 0)
		   && (memcmp (plt_contents + 6,
			       lazy_bnd_plt->plt0_entry + 6, 3) == 0))
	    {
	      plt_type = plt_lazy | plt_second;
	      /* The fist entry in the lazy IBT PLT is the same as the
		 lazy BND PLT.  */
	      if ((memcmp (plt_contents + lazy_ibt_plt->plt_entry_size,
			   lazy_ibt_plt->plt_entry,
			   lazy_ibt_plt->plt_got_offset) == 0))
		lazy_plt = lazy_ibt_plt;
	      else
		lazy_plt = lazy_bnd_plt;
	    }
	}

      if (non_lazy_plt != NULL
	  && (plt_type == plt_unknown || plt_type == plt_non_lazy)
	  && plt->size >= non_lazy_plt->plt_entry_size)
	{
	  /* Match non-lazy PLT.  */
	  if (memcmp (plt_contents, non_lazy_plt->plt_entry,
		      non_lazy_plt->plt_got_offset) == 0)
	    plt_type = plt_non_lazy;
	}

      if (plt_type == plt_unknown || plt_type == plt_second)
	{
	  if (non_lazy_bnd_plt != NULL
	      && plt->size >= non_lazy_bnd_plt->plt_entry_size
	      && (memcmp (plt_contents, non_lazy_bnd_plt->plt_entry,
			  non_lazy_bnd_plt->plt_got_offset) == 0))
	    {
	      /* Match BND PLT.  */
	      plt_type = plt_second;
	      non_lazy_plt = non_lazy_bnd_plt;
	    }
	  else if (non_lazy_ibt_plt != NULL
		   && plt->size >= non_lazy_ibt_plt->plt_entry_size
		   && (memcmp (plt_contents,
			       non_lazy_ibt_plt->plt_entry,
			       non_lazy_ibt_plt->plt_got_offset) == 0))
	    {
	      /* Match IBT PLT.  */
	      plt_type = plt_second;
	      non_lazy_plt = non_lazy_ibt_plt;
	    }
	}

      if (plt_type == plt_unknown)
	continue;

      plts[j].sec = plt;
      plts[j].type = plt_type;

      if ((plt_type & plt_lazy))
	{
	  plts[j].plt_got_offset = lazy_plt->plt_got_offset;
	  plts[j].plt_got_insn_size = lazy_plt->plt_got_insn_size;
	  plts[j].plt_entry_size = lazy_plt->plt_entry_size;
	  /* Skip PLT0 in lazy PLT.  */
	  i = 1;
	}
      else
	{
	  plts[j].plt_got_offset = non_lazy_plt->plt_got_offset;
	  plts[j].plt_got_insn_size = non_lazy_plt->plt_got_insn_size;
	  plts[j].plt_entry_size = non_lazy_plt->plt_entry_size;
	  i = 0;
	}

      /* Skip lazy PLT when the second PLT is used.  */
      if (plt_type == (plt_lazy | plt_second))
	plts[j].count = 0;
      else
	{
	  n = plt->size / plts[j].plt_entry_size;
	  plts[j].count = n;
	  count += n - i;
	}

      plts[j].contents = plt_contents;
    }

  if (count == 0)
    return -1;

  size = count * sizeof (asymbol);
  s = *ret = (asymbol *) bfd_zmalloc (size);
  if (s == NULL)
    {
bad_return:
      for (j = 0; plts[j].name != NULL; j++)
	if (plts[j].contents != NULL)
	  free (plts[j].contents);
      free (dynrelbuf);
      return -1;
    }

  /* Check for each PLT section.  */
  size = 0;
  n = 0;
  for (j = 0; plts[j].name != NULL; j++)
    if ((plt_contents = plts[j].contents) != NULL)
      {
	long k;
	bfd_vma offset;

	plt_got_offset = plts[j].plt_got_offset;
	plt_got_insn_size = plts[j].plt_got_insn_size;
	plt_entry_size = plts[j].plt_entry_size;

	plt = plts[j].sec;

	if ((plts[j].type & plt_lazy))
	  {
	    /* Skip PLT0 in lazy PLT.  */
	    k = 1;
	    offset = plt_entry_size;
	  }
	else
	  {
	    k = 0;
	    offset = 0;
	  }

	/* Check each PLT entry against dynamic relocations.  */
	for (; k < plts[j].count; k++)
	  {
	    int off;
	    bfd_vma got_vma;
	    long min, max, mid;
	    arelent *p;

	    /* Get the PC-relative offset, a signed 32-bit integer.  */
	    off = H_GET_32 (abfd, (plt_contents + offset
				   + plt_got_offset));
	    got_vma = plt->vma + offset + off + plt_got_insn_size;

	    /* Binary search.  */
	    p = dynrelbuf[0];
	    min = 0;
	    max = dynrelcount;
	    while ((min + 1) < max)
	      {
		arelent *r;

		mid = (min + max) / 2;
		r = dynrelbuf[mid];
		if (got_vma > r->address)
		  min = mid;
		else if (got_vma < r->address)
		  max = mid;
		else
		  {
		    p = r;
		    break;
		  }
	      }

	    /* Skip unknown relocation.  PR 17512: file: bc9d6cf5.  */
	    if (got_vma == p->address
		&& p->howto != NULL
		&& (p->howto->type == R_X86_64_JUMP_SLOT
		    || p->howto->type == R_X86_64_GLOB_DAT
		    || p->howto->type == R_X86_64_IRELATIVE))
	      {
		*s = **p->sym_ptr_ptr;
		/* Undefined syms won't have BSF_LOCAL or BSF_GLOBAL
		   set.  Since we are defining a symbol, ensure one
		   of them is set.  */
		if ((s->flags & BSF_LOCAL) == 0)
		  s->flags |= BSF_GLOBAL;
		s->flags |= BSF_SYNTHETIC;
		/* This is no longer a section symbol.  */
		s->flags &= ~BSF_SECTION_SYM;
		s->section = plt;
		s->the_bfd = plt->owner;
		s->value = offset;
		/* Store relocation for later use.  */
		s->udata.p = p;
		/* Add @plt to function name later.  */
		size += strlen (s->name) + sizeof ("@plt");
		if (p->addend != 0)
		  size += sizeof ("+0x") - 1 + 8 + 8 * ABI_64_P (abfd);
		n++;
		s++;
	      }
	    offset += plt_entry_size;
	  }
      }

  /* PLT entries with R_X86_64_TLSDESC relocations are skipped.  */
  if (n == 0)
    goto bad_return;

  count = n;

  /* Allocate space for @plt suffixes.  */
  names = (char *) bfd_malloc (size);
  if (s == NULL)
    goto bad_return;

  s = *ret;
  for (i = 0; i < count; i++)
    {
      /* Add @plt to function name.  */
      arelent *p = (arelent *) s->udata.p;
      /* Clear it now.  */
      s->udata.p = NULL;
      size = strlen (s->name);
      memcpy (names, s->name, size);
      s->name = names;
      names += size;
      if (p->addend != 0)
	{
	  char buf[30], *a;

	  memcpy (names, "+0x", sizeof ("+0x") - 1);
	  names += sizeof ("+0x") - 1;
	  bfd_sprintf_vma (abfd, buf, p->addend);
	  for (a = buf; *a == '0'; ++a)
	    ;
	  size = strlen (a);
	  memcpy (names, a, size);
	  names += size;
	}
      memcpy (names, "@plt", sizeof ("@plt"));
      names += sizeof ("@plt");
      s++;
    }

  for (j = 0; plts[j].name != NULL; j++)
    if (plts[j].contents != NULL)
      free (plts[j].contents);

  free (dynrelbuf);

  return count;
}