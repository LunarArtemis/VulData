static long
elf_i386_get_synthetic_symtab (bfd *abfd,
			       long symcount ATTRIBUTE_UNUSED,
			       asymbol **syms ATTRIBUTE_UNUSED,
			       long dynsymcount,
			       asymbol **dynsyms,
			       asymbol **ret)
{
  long size, count, i, n;
  int j;
  unsigned int plt_got_offset, plt_entry_size;
  asymbol *s;
  bfd_byte *plt_contents;
  long dynrelcount, relsize;
  arelent **dynrelbuf;
  const struct elf_i386_lazy_plt_layout *lazy_plt;
  const struct elf_i386_non_lazy_plt_layout *non_lazy_plt;
  const struct elf_i386_lazy_plt_layout *lazy_ibt_plt;
  const struct elf_i386_non_lazy_plt_layout *non_lazy_ibt_plt;
  asection *plt;
  bfd_vma got_addr;
  char *names;
  enum elf_i386_plt_type plt_type;
  struct elf_i386_plt plts[] =
    {
      { ".plt", NULL, NULL, plt_unknown, 0, 0, 0 },
      { ".plt.got", NULL, NULL, plt_non_lazy, 0, 0, 0 },
      { ".plt.sec", NULL, NULL, plt_second, 0, 0, 0 },
      { NULL, NULL, NULL, plt_non_lazy, 0, 0, 0 }
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

  non_lazy_plt = NULL;
  /* Silence GCC 6.  */
  lazy_plt = NULL;
  non_lazy_ibt_plt = NULL;
  lazy_ibt_plt = NULL;
  switch (get_elf_i386_backend_data (abfd)->os)
    {
    case is_normal:
      non_lazy_plt = &elf_i386_non_lazy_plt;
      lazy_ibt_plt = &elf_i386_lazy_ibt_plt;
      non_lazy_ibt_plt = &elf_i386_non_lazy_ibt_plt;
      /* Fall through */
    case is_vxworks:
      lazy_plt = &elf_i386_lazy_plt;
      break;
    case is_nacl:
      lazy_plt = &elf_i386_nacl_plt;
      break;
    }

  got_addr = 0;

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
	  && (plt->size >= (lazy_plt->plt0_entry_size
			    + lazy_plt->plt_entry_size)))
	{
	  /* Match lazy PLT first.  */
	  if (memcmp (plt_contents, lazy_plt->plt0_entry,
		      lazy_plt->plt0_got1_offset) == 0)
	    {
	      /* The fist entry in the lazy IBT PLT is the same as the
		 normal lazy PLT.  */
	      if (lazy_ibt_plt != NULL
		  && (memcmp (plt_contents + lazy_ibt_plt->plt0_entry_size,
			      lazy_ibt_plt->plt_entry,
			      lazy_ibt_plt->plt_got_offset) == 0))
		plt_type = plt_lazy | plt_second;
	      else
		plt_type = plt_lazy;
	    }
	  else if (memcmp (plt_contents, lazy_plt->pic_plt0_entry,
			   lazy_plt->plt0_got1_offset) == 0)
	    {
	      /* The fist entry in the PIC lazy IBT PLT is the same as
		 the normal PIC lazy PLT.  */
	      if (lazy_ibt_plt != NULL
		  && (memcmp (plt_contents + lazy_ibt_plt->plt0_entry_size,
			      lazy_ibt_plt->pic_plt_entry,
			      lazy_ibt_plt->plt_got_offset) == 0))
		plt_type = plt_lazy | plt_pic | plt_second;
	      else
		plt_type = plt_lazy | plt_pic;
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
	  else if (memcmp (plt_contents, non_lazy_plt->pic_plt_entry,
			   non_lazy_plt->plt_got_offset) == 0)
	    plt_type = plt_pic;
	}

      if ((non_lazy_ibt_plt != NULL)
	  && (plt_type == plt_unknown || plt_type == plt_second)
	  && plt->size >= non_lazy_ibt_plt->plt_entry_size)
	{
	  if (memcmp (plt_contents,
		      non_lazy_ibt_plt->plt_entry,
		      non_lazy_ibt_plt->plt_got_offset) == 0)
	    {
	      /* Match IBT PLT.  */
	      plt_type = plt_second;
	      non_lazy_plt = non_lazy_ibt_plt;
	    }
	  else if (memcmp (plt_contents,
			   non_lazy_ibt_plt->pic_plt_entry,
			   non_lazy_ibt_plt->plt_got_offset) == 0)
	    {
	      /* Match PIC IBT PLT.  */
	      plt_type = plt_second | plt_pic;
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
	  plts[j].plt_entry_size = lazy_plt->plt_entry_size;
	  /* Skip PLT0 in lazy PLT.  */
	  i = 1;
	}
      else
	{
	  plts[j].plt_got_offset = non_lazy_plt->plt_got_offset;
	  plts[j].plt_entry_size = non_lazy_plt->plt_entry_size;
	  i = 0;
	}

      /* Skip lazy PLT when the second PLT is used.  */
      if ((plt_type & (plt_lazy | plt_second))
	  == (plt_lazy | plt_second))
	plts[j].count = 0;
      else
	{
	  n = plt->size / plts[j].plt_entry_size;
	  plts[j].count = n;
	  count += n - i;
	}

      plts[j].contents = plt_contents;

      /* The _GLOBAL_OFFSET_TABLE_ address is needed.  */
      if ((plt_type & plt_pic))
	got_addr = (bfd_vma) -1;
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

  if (got_addr)
    {
      /* Check .got.plt and then .got to get the _GLOBAL_OFFSET_TABLE_
	 address.  */
      asection *sec = bfd_get_section_by_name (abfd, ".got.plt");
      if (sec != NULL)
	got_addr = sec->vma;
      else
	{
	  sec = bfd_get_section_by_name (abfd, ".got");
	  if (sec != NULL)
	    got_addr = sec->vma;
	}

      if (got_addr == (bfd_vma) -1)
	goto bad_return;
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

	    /* Get the GOT offset, a signed 32-bit integer.  */
	    off = H_GET_32 (abfd, (plt_contents + offset
				   + plt_got_offset));
	    got_vma = got_addr + off;

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
		&& (p->howto->type == R_386_JUMP_SLOT
		    || p->howto->type == R_386_GLOB_DAT
		    || p->howto->type == R_386_IRELATIVE))
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
		  size += sizeof ("+0x") - 1 + 8;
		n++;
		s++;
	      }
	    offset += plt_entry_size;
	  }
      }

  /* PLT entries with R_386_TLS_DESC relocations are skipped.  */
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