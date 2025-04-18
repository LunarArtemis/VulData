long
_bfd_x86_elf_get_synthetic_symtab (bfd *abfd,
				   long count,
				   long relsize,
				   bfd_vma got_addr,
				   struct elf_x86_plt plts[],
				   asymbol **dynsyms,
				   asymbol **ret)
{
  long size, i, n, len;
  int j;
  unsigned int plt_got_offset, plt_entry_size;
  asymbol *s;
  bfd_byte *plt_contents;
  long dynrelcount;
  arelent **dynrelbuf, *p;
  char *names;
  const struct elf_backend_data *bed;
  bfd_vma (*get_plt_got_vma) (struct elf_x86_plt *, bfd_vma, bfd_vma,
			      bfd_vma);
  bfd_boolean (*valid_plt_reloc_p) (unsigned int);

  if (count == 0)
    return -1;

  dynrelbuf = (arelent **) bfd_malloc (relsize);
  if (dynrelbuf == NULL)
    return -1;

  dynrelcount = bfd_canonicalize_dynamic_reloc (abfd, dynrelbuf,
						dynsyms);
  if (dynrelcount <= 0)
    return -1;

  /* Sort the relocs by address.  */
  qsort (dynrelbuf, dynrelcount, sizeof (arelent *),
	 _bfd_x86_elf_compare_relocs);

  size = count * sizeof (asymbol);

  /* Allocate space for @plt suffixes.  */
  n = 0;
  for (i = 0; i < dynrelcount; i++)
    {
      p = dynrelbuf[i];
      size += strlen ((*p->sym_ptr_ptr)->name) + sizeof ("@plt");
      if (p->addend != 0)
	size += sizeof ("+0x") - 1 + 8 + 8 * ABI_64_P (abfd);
    }

  s = *ret = (asymbol *) bfd_zmalloc (size);
  if (s == NULL)
    goto bad_return;

  bed = get_elf_backend_data (abfd);

  if (bed->target_id == X86_64_ELF_DATA)
    {
      get_plt_got_vma = elf_x86_64_get_plt_got_vma;
      valid_plt_reloc_p = elf_x86_64_valid_plt_reloc_p;
    }
  else
    {
      get_plt_got_vma = elf_i386_get_plt_got_vma;
      valid_plt_reloc_p = elf_i386_valid_plt_reloc_p;
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
    }

  /* Check for each PLT section.  */
  names = (char *) (s + count);
  size = 0;
  n = 0;
  for (j = 0; plts[j].name != NULL; j++)
    if ((plt_contents = plts[j].contents) != NULL)
      {
	long k;
	bfd_vma offset;
	asection *plt;
	struct elf_x86_plt *plt_p = &plts[j];

	plt_got_offset = plt_p->plt_got_offset;
	plt_entry_size = plt_p->plt_entry_size;

	plt = plt_p->sec;

	if ((plt_p->type & plt_lazy))
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
	for (; k < plt_p->count; k++)
	  {
	    int off;
	    bfd_vma got_vma;
	    long min, max, mid;

	    /* Get the GOT offset for i386 or the PC-relative offset
	       for x86-64, a signed 32-bit integer.  */
	    off = H_GET_32 (abfd, (plt_contents + offset
				   + plt_got_offset));
	    got_vma = get_plt_got_vma (plt_p, off, offset, got_addr);

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
		&& valid_plt_reloc_p (p->howto->type))
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
		s->udata.p = NULL;
		s->name = names;
		len = strlen ((*p->sym_ptr_ptr)->name);
		memcpy (names, (*p->sym_ptr_ptr)->name, len);
		names += len;
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
		n++;
		s++;
	      }
	    offset += plt_entry_size;
	  }
      }

  /* PLT entries with R_386_TLS_DESC relocations are skipped.  */
  if (n == 0)
    {
bad_return:
      count = -1;
    }
  else
    count = n;

  for (j = 0; plts[j].name != NULL; j++)
    if (plts[j].contents != NULL)
      free (plts[j].contents);

  free (dynrelbuf);

  return count;
}