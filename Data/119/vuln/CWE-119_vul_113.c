static struct comp_unit *
parse_comp_unit (struct dwarf2_debug *stash,
		 bfd_vma unit_length,
		 bfd_byte *info_ptr_unit,
		 unsigned int offset_size)
{
  struct comp_unit* unit;
  unsigned int version;
  bfd_uint64_t abbrev_offset = 0;
  /* Initialize it just to avoid a GCC false warning.  */
  unsigned int addr_size = -1;
  struct abbrev_info** abbrevs;
  unsigned int abbrev_number, bytes_read, i;
  struct abbrev_info *abbrev;
  struct attribute attr;
  bfd_byte *info_ptr = stash->info_ptr;
  bfd_byte *end_ptr = info_ptr + unit_length;
  bfd_size_type amt;
  bfd_vma low_pc = 0;
  bfd_vma high_pc = 0;
  bfd *abfd = stash->bfd_ptr;
  bfd_boolean high_pc_relative = FALSE;
  enum dwarf_unit_type unit_type;

  version = read_2_bytes (abfd, info_ptr, end_ptr);
  info_ptr += 2;
  if (version < 2 || version > 5)
    {
      /* PR 19872: A version number of 0 probably means that there is padding
	 at the end of the .debug_info section.  Gold puts it there when
	 performing an incremental link, for example.  So do not generate
	 an error, just return a NULL.  */
      if (version)
	{
	  _bfd_error_handler
	    (_("Dwarf Error: found dwarf version '%u', this reader"
	       " only handles version 2, 3, 4 and 5 information."), version);
	  bfd_set_error (bfd_error_bad_value);
	}
      return NULL;
    }

  if (version < 5)
    unit_type = DW_UT_compile;
  else
    {
      unit_type = read_1_byte (abfd, info_ptr, end_ptr);
      info_ptr += 1;

      addr_size = read_1_byte (abfd, info_ptr, end_ptr);
      info_ptr += 1;
    }

  BFD_ASSERT (offset_size == 4 || offset_size == 8);
  if (offset_size == 4)
    abbrev_offset = read_4_bytes (abfd, info_ptr, end_ptr);
  else
    abbrev_offset = read_8_bytes (abfd, info_ptr, end_ptr);
  info_ptr += offset_size;

  if (version < 5)
    {
      addr_size = read_1_byte (abfd, info_ptr, end_ptr);
      info_ptr += 1;
    }

  if (unit_type == DW_UT_type)
    {
      /* Skip type signature.  */
      info_ptr += 8;

      /* Skip type offset.  */
      info_ptr += offset_size;
    }

  if (addr_size > sizeof (bfd_vma))
    {
      _bfd_error_handler
	/* xgettext: c-format */
	(_("Dwarf Error: found address size '%u', this reader"
	   " can not handle sizes greater than '%u'."),
	 addr_size,
	 (unsigned int) sizeof (bfd_vma));
      bfd_set_error (bfd_error_bad_value);
      return NULL;
    }

  if (addr_size != 2 && addr_size != 4 && addr_size != 8)
    {
      _bfd_error_handler
	("Dwarf Error: found address size '%u', this reader"
	 " can only handle address sizes '2', '4' and '8'.", addr_size);
      bfd_set_error (bfd_error_bad_value);
      return NULL;
    }

  /* Read the abbrevs for this compilation unit into a table.  */
  abbrevs = read_abbrevs (abfd, abbrev_offset, stash);
  if (! abbrevs)
    return NULL;

  abbrev_number = _bfd_safe_read_leb128 (abfd, info_ptr, &bytes_read,
					 FALSE, end_ptr);
  info_ptr += bytes_read;
  if (! abbrev_number)
    {
      /* PR 19872: An abbrev number of 0 probably means that there is padding
	 at the end of the .debug_abbrev section.  Gold puts it there when
	 performing an incremental link, for example.  So do not generate
	 an error, just return a NULL.  */
      return NULL;
    }

  abbrev = lookup_abbrev (abbrev_number, abbrevs);
  if (! abbrev)
    {
      _bfd_error_handler (_("Dwarf Error: Could not find abbrev number %u."),
			  abbrev_number);
      bfd_set_error (bfd_error_bad_value);
      return NULL;
    }

  amt = sizeof (struct comp_unit);
  unit = (struct comp_unit *) bfd_zalloc (abfd, amt);
  if (unit == NULL)
    return NULL;
  unit->abfd = abfd;
  unit->version = version;
  unit->addr_size = addr_size;
  unit->offset_size = offset_size;
  unit->abbrevs = abbrevs;
  unit->end_ptr = end_ptr;
  unit->stash = stash;
  unit->info_ptr_unit = info_ptr_unit;
  unit->sec_info_ptr = stash->sec_info_ptr;

  for (i = 0; i < abbrev->num_attrs; ++i)
    {
      info_ptr = read_attribute (&attr, &abbrev->attrs[i], unit, info_ptr, end_ptr);
      if (info_ptr == NULL)
	return NULL;

      /* Store the data if it is of an attribute we want to keep in a
	 partial symbol table.  */
      switch (attr.name)
	{
	case DW_AT_stmt_list:
	  unit->stmtlist = 1;
	  unit->line_offset = attr.u.val;
	  break;

	case DW_AT_name:
	  if (is_str_attr (attr.form))
	    unit->name = attr.u.str;
	  break;

	case DW_AT_low_pc:
	  low_pc = attr.u.val;
	  /* If the compilation unit DIE has a DW_AT_low_pc attribute,
	     this is the base address to use when reading location
	     lists or range lists.  */
	  if (abbrev->tag == DW_TAG_compile_unit)
	    unit->base_address = low_pc;
	  break;

	case DW_AT_high_pc:
	  high_pc = attr.u.val;
	  high_pc_relative = attr.form != DW_FORM_addr;
	  break;

	case DW_AT_ranges:
	  if (!read_rangelist (unit, &unit->arange, attr.u.val))
	    return NULL;
	  break;

	case DW_AT_comp_dir:
	  {
	    char *comp_dir = attr.u.str;

	    /* PR 17512: file: 1fe726be.  */
	    if (! is_str_attr (attr.form))
	      {
		_bfd_error_handler
		  (_("Dwarf Error: DW_AT_comp_dir attribute encountered with a non-string form."));
		comp_dir = NULL;
	      }

	    if (comp_dir)
	      {
		/* Irix 6.2 native cc prepends <machine>.: to the compilation
		   directory, get rid of it.  */
		char *cp = strchr (comp_dir, ':');

		if (cp && cp != comp_dir && cp[-1] == '.' && cp[1] == '/')
		  comp_dir = cp + 1;
	      }
	    unit->comp_dir = comp_dir;
	    break;
	  }

	case DW_AT_language:
	  unit->lang = attr.u.val;
	  break;

	default:
	  break;
	}
    }
  if (high_pc_relative)
    high_pc += low_pc;
  if (high_pc != 0)
    {
      if (!arange_add (unit, &unit->arange, low_pc, high_pc))
	return NULL;
    }

  unit->first_child_die_ptr = info_ptr;
  return unit;
}