bfd_boolean
load_debug_section (enum dwarf_section_display_enum debug, void * data)
{
  struct dwarf_section * section = &debug_displays [debug].section;
  Elf_Internal_Shdr * sec;
  Filedata * filedata = (Filedata *) data;

  if (filedata->string_table == NULL)
    {
      Elf_Internal_Shdr * strs;

      /* Read in the string table, so that we have section names to scan.  */
      strs = filedata->section_headers + filedata->file_header.e_shstrndx;

      if (strs != NULL && strs->sh_size != 0)
	{
	  filedata->string_table = (char *) get_data (NULL, filedata, strs->sh_offset,
						      1, strs->sh_size,
						      _("string table"));

	  filedata->string_table_length = filedata->string_table != NULL ? strs->sh_size : 0;
	}
    }

  /* Locate the debug section.  */
  sec = find_section_in_set (filedata, section->uncompressed_name, section_subset);
  if (sec != NULL)
    section->name = section->uncompressed_name;
  else
    {
      sec = find_section_in_set (filedata, section->compressed_name, section_subset);
      if (sec != NULL)
	section->name = section->compressed_name;
    }
  if (sec == NULL)
    return FALSE;

  /* If we're loading from a subset of sections, and we've loaded
     a section matching this name before, it's likely that it's a
     different one.  */
  if (section_subset != NULL)
    free_debug_section (debug);

  return load_specific_debug_section (debug, sec, data);
}