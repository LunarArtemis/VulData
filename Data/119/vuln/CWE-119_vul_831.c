static int unzzip_cat (int argc, char ** argv, int extract)
{
    int argn;
    FILE* disk;

    disk = fopen (argv[1], "r");
    if (! disk) {
	perror(argv[1]);
	return exitcode(errno);
    }

    if (argc == 2)
    {  /* print directory list */
	ZZIP_ENTRY* entry = zzip_entry_findfirst(disk);
	for (; entry ; entry = zzip_entry_findnext(entry))
	{
	    char* name = zzip_entry_strdup_name (entry);
	    FILE* out = stdout;
	    if (extract) out = create_fopen(name, "w", 1);
	    unzzip_cat_file (disk, name, out);
	    if (extract) fclose(out);
	    free (name);
	}
	return 0;
    }

    if (argc == 3 && !extract)
    {  /* list from one spec */
	ZZIP_ENTRY* entry = 0;
	while ((entry = zzip_entry_findmatch(disk, argv[2], entry, 0, 0)))
	{
	     unzzip_big_entry_fprint (entry, stdout);
	}
	return 0;
    }

    for (argn=1; argn < argc; argn++)
    {   /* list only the matching entries - each in order of commandline */
	ZZIP_ENTRY* entry = zzip_entry_findfirst(disk);
	for (; entry ; entry = zzip_entry_findnext(entry))
	{
	    char* name = zzip_entry_strdup_name (entry);
	    DBG3(".. check '%s' to zip '%s'", argv[argn], name);
	    if (! _zzip_fnmatch (argv[argn], name, 
		FNM_NOESCAPE|FNM_PATHNAME|FNM_PERIOD))
	    {
	        FILE* out = stdout;
	        if (extract) out = create_fopen(name, "w", 1);
		unzzip_cat_file (disk, name, out);
		if (extract) fclose(out);
		break; /* match loop */
	    }
	    free (name);
	}
    }
    return 0;
}