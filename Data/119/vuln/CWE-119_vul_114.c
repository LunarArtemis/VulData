static bfd_boolean
find_abstract_instance_name (struct comp_unit *unit,
			     bfd_byte *orig_info_ptr,
			     struct attribute *attr_ptr,
			     const char **pname,
			     bfd_boolean *is_linkage)
{
  bfd *abfd = unit->abfd;
  bfd_byte *info_ptr;
  bfd_byte *info_ptr_end;
  unsigned int abbrev_number, bytes_read, i;
  struct abbrev_info *abbrev;
  bfd_uint64_t die_ref = attr_ptr->u.val;
  struct attribute attr;
  const char *name = NULL;

  /* DW_FORM_ref_addr can reference an entry in a different CU. It
     is an offset from the .debug_info section, not the current CU.  */
  if (attr_ptr->form == DW_FORM_ref_addr)
    {
      /* We only support DW_FORM_ref_addr within the same file, so
	 any relocations should be resolved already.  */
      if (!die_ref)
	{
	  _bfd_error_handler
	    (_("Dwarf Error: Abstract instance DIE ref zero."));
	  bfd_set_error (bfd_error_bad_value);
	  return FALSE;
	}

      info_ptr = unit->sec_info_ptr + die_ref;
      info_ptr_end = unit->end_ptr;

      /* Now find the CU containing this pointer.  */
      if (info_ptr >= unit->info_ptr_unit && info_ptr < unit->end_ptr)
	;
      else
	{
	  /* Check other CUs to see if they contain the abbrev.  */
	  struct comp_unit * u;

	  for (u = unit->prev_unit; u != NULL; u = u->prev_unit)
	    if (info_ptr >= u->info_ptr_unit && info_ptr < u->end_ptr)
	      break;

	  if (u == NULL)
	    for (u = unit->next_unit; u != NULL; u = u->next_unit)
	      if (info_ptr >= u->info_ptr_unit && info_ptr < u->end_ptr)
		break;

	  if (u)
	    unit = u;
	  /* else FIXME: What do we do now ?  */
	}
    }
  else if (attr_ptr->form == DW_FORM_GNU_ref_alt)
    {
      info_ptr = read_alt_indirect_ref (unit, die_ref);
      if (info_ptr == NULL)
	{
	  _bfd_error_handler
	    (_("Dwarf Error: Unable to read alt ref %llu."),
	     (long long) die_ref);
	  bfd_set_error (bfd_error_bad_value);
	  return FALSE;
	}
      info_ptr_end = (unit->stash->alt_dwarf_info_buffer
		      + unit->stash->alt_dwarf_info_size);

      /* FIXME: Do we need to locate the correct CU, in a similar
	 fashion to the code in the DW_FORM_ref_addr case above ?  */
    }
  else
    {
      info_ptr = unit->info_ptr_unit + die_ref;
      info_ptr_end = unit->end_ptr;
    }

  abbrev_number = _bfd_safe_read_leb128 (abfd, info_ptr, &bytes_read,
					 FALSE, info_ptr_end);
  info_ptr += bytes_read;

  if (abbrev_number)
    {
      abbrev = lookup_abbrev (abbrev_number, unit->abbrevs);
      if (! abbrev)
	{
	  _bfd_error_handler
	    (_("Dwarf Error: Could not find abbrev number %u."), abbrev_number);
	  bfd_set_error (bfd_error_bad_value);
	  return FALSE;
	}
      else
	{
	  for (i = 0; i < abbrev->num_attrs; ++i)
	    {
	      info_ptr = read_attribute (&attr, &abbrev->attrs[i], unit,
					 info_ptr, info_ptr_end);
	      if (info_ptr == NULL)
		break;
	      /* It doesn't ever make sense for DW_AT_specification to
		 refer to the same DIE.  Stop simple recursion.  */
	      if (info_ptr == orig_info_ptr)
		{
		  _bfd_error_handler
		    (_("Dwarf Error: Abstract instance recursion detected."));
		  bfd_set_error (bfd_error_bad_value);
		  return FALSE;
		}
	      switch (attr.name)
		{
		case DW_AT_name:
		  /* Prefer DW_AT_MIPS_linkage_name or DW_AT_linkage_name
		     over DW_AT_name.  */
		  if (name == NULL && is_str_attr (attr.form))
		    {
		      name = attr.u.str;
		      if (non_mangled (unit->lang))
			*is_linkage = TRUE;
		    }
		  break;
		case DW_AT_specification:
		  if (!find_abstract_instance_name (unit, info_ptr, &attr,
						    pname, is_linkage))
		    return FALSE;
		  break;
		case DW_AT_linkage_name:
		case DW_AT_MIPS_linkage_name:
		  /* PR 16949:  Corrupt debug info can place
		     non-string forms into these attributes.  */
		  if (is_str_attr (attr.form))
		    {
		      name = attr.u.str;
		      *is_linkage = TRUE;
		    }
		  break;
		default:
		  break;
		}
	    }
	}
    }
  *pname = name;
  return TRUE;
}