char *
get_archive_member_name (struct archive_info *arch,
                         struct archive_info *nested_arch)
{
  unsigned long j, k;

  if (arch->arhdr.ar_name[0] == '/')
    {
      /* We have a long name.  */
      char *endp;
      char *member_file_name;
      char *member_name;

      if (arch->longnames == NULL || arch->longnames_size == 0)
	{
	  error (_("Archive member uses long names, but no longname table found\n"));
	  return NULL;
	}

      arch->nested_member_origin = 0;
      k = j = strtoul (arch->arhdr.ar_name + 1, &endp, 10);
      if (arch->is_thin_archive && endp != NULL && * endp == ':')
        arch->nested_member_origin = strtoul (endp + 1, NULL, 10);

      if (j > arch->longnames_size)
	{
	  error (_("Found long name index (%ld) beyond end of long name table\n"),j);
	  return NULL;
	}
      while ((j < arch->longnames_size)
             && (arch->longnames[j] != '\n')
             && (arch->longnames[j] != '\0'))
        j++;
      if (j > 0 && arch->longnames[j-1] == '/')
        j--;
      if (j > arch->longnames_size)
	j = arch->longnames_size;
      arch->longnames[j] = '\0';

      if (!arch->is_thin_archive || arch->nested_member_origin == 0)
        return arch->longnames + k;

      /* PR 17531: file: 2896dc8b.  */
      if (k >= j)
	{
	  error (_("Invalid Thin archive member name\n"));
	  return NULL;
	}

      /* This is a proxy for a member of a nested archive.
         Find the name of the member in that archive.  */
      member_file_name = adjust_relative_path (arch->file_name,
					       arch->longnames + k, j - k);
      if (member_file_name != NULL
          && setup_nested_archive (nested_arch, member_file_name) == 0)
	{
          member_name = get_archive_member_name_at (nested_arch,
						    arch->nested_member_origin,
						    NULL);
	  if (member_name != NULL)
	    {
	      free (member_file_name);
	      return member_name;
	    }
	}
      free (member_file_name);

      /* Last resort: just return the name of the nested archive.  */
      return arch->longnames + k;
    }

  /* We have a normal (short) name.  */
  for (j = 0; j < sizeof (arch->arhdr.ar_name); j++)
    if (arch->arhdr.ar_name[j] == '/')
      {
	arch->arhdr.ar_name[j] = '\0';
	return arch->arhdr.ar_name;
      }

  /* The full ar_name field is used.  Don't rely on ar_date starting
     with a zero byte.  */
  {
    char *name = xmalloc (sizeof (arch->arhdr.ar_name) + 1);
    memcpy (name, arch->arhdr.ar_name, sizeof (arch->arhdr.ar_name));
    name[sizeof (arch->arhdr.ar_name)] = '\0';
    return name;
  }
}