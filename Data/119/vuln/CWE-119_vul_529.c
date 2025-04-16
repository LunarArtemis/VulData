static int
glob_in_dir (const char *pattern, const char *directory, int flags,
	     int (*errfunc) (const char *, int),
	     glob_t *pglob, size_t alloca_used)
{
  size_t dirlen = strlen (directory);
  void *stream = NULL;
  struct globnames
    {
      struct globnames *next;
      size_t count;
      char *name[64];
    };
#define INITIAL_COUNT sizeof (init_names.name) / sizeof (init_names.name[0])
  struct globnames init_names;
  struct globnames *names = &init_names;
  struct globnames *names_alloca = &init_names;
  size_t nfound = 0;
  size_t cur = 0;
  int meta;
  int save;

  alloca_used += sizeof (init_names);

  init_names.next = NULL;
  init_names.count = INITIAL_COUNT;

  meta = __glob_pattern_type (pattern, !(flags & GLOB_NOESCAPE));
  if (meta == 0 && (flags & (GLOB_NOCHECK|GLOB_NOMAGIC)))
    {
      /* We need not do any tests.  The PATTERN contains no meta
	 characters and we must not return an error therefore the
	 result will always contain exactly one name.  */
      flags |= GLOB_NOCHECK;
    }
  else if (meta == 0)
    {
      /* Since we use the normal file functions we can also use stat()
	 to verify the file is there.  */
      union
      {
	struct stat st;
	struct_stat64 st64;
      } ust;
      size_t patlen = strlen (pattern);
      int alloca_fullname = __libc_use_alloca (alloca_used
					       + dirlen + 1 + patlen + 1);
      char *fullname;
      if (alloca_fullname)
	fullname = alloca_account (dirlen + 1 + patlen + 1, alloca_used);
      else
	{
	  fullname = malloc (dirlen + 1 + patlen + 1);
	  if (fullname == NULL)
	    return GLOB_NOSPACE;
	}

      mempcpy (mempcpy (mempcpy (fullname, directory, dirlen),
			"/", 1),
	       pattern, patlen + 1);
      if ((__builtin_expect (flags & GLOB_ALTDIRFUNC, 0)
	   ? (*pglob->gl_stat) (fullname, &ust.st)
	   : __stat64 (fullname, &ust.st64)) == 0)
	/* We found this file to be existing.  Now tell the rest
	   of the function to copy this name into the result.  */
	flags |= GLOB_NOCHECK;

      if (__glibc_unlikely (!alloca_fullname))
	free (fullname);
    }
  else
    {
      stream = (__builtin_expect (flags & GLOB_ALTDIRFUNC, 0)
		? (*pglob->gl_opendir) (directory)
		: opendir (directory));
      if (stream == NULL)
	{
	  if (errno != ENOTDIR
	      && ((errfunc != NULL && (*errfunc) (directory, errno))
		  || (flags & GLOB_ERR)))
	    return GLOB_ABORTED;
	}
      else
	{
#ifdef _LIBC
	  int dfd = (__builtin_expect (flags & GLOB_ALTDIRFUNC, 0)
		     ? -1 : dirfd ((DIR *) stream));
#endif
	  int fnm_flags = ((!(flags & GLOB_PERIOD) ? FNM_PERIOD : 0)
			   | ((flags & GLOB_NOESCAPE) ? FNM_NOESCAPE : 0)
#if defined _AMIGA || defined VMS
			   | FNM_CASEFOLD
#endif
			   );
	  flags |= GLOB_MAGCHAR;

	  while (1)
	    {
	      const char *name;
#if defined _LIBC && !defined COMPILE_GLOB64
	      struct dirent64 *d;
	      union
		{
		  struct dirent64 d64;
		  char room [offsetof (struct dirent64, d_name[0])
			     + NAME_MAX + 1];
		}
	      d64buf;

	      if (__glibc_unlikely (flags & GLOB_ALTDIRFUNC))
		{
		  struct dirent *d32 = (*pglob->gl_readdir) (stream);
		  if (d32 != NULL)
		    {
		      CONVERT_DIRENT_DIRENT64 (&d64buf.d64, d32);
		      d = &d64buf.d64;
		    }
		  else
		    d = NULL;
		}
	      else
		d = __readdir64 (stream);
#else
	      struct dirent *d = (__builtin_expect (flags & GLOB_ALTDIRFUNC, 0)
				  ? ((struct dirent *)
				     (*pglob->gl_readdir) (stream))
				  : __readdir (stream));
#endif
	      if (d == NULL)
		break;
	      if (! REAL_DIR_ENTRY (d))
		continue;

	      /* If we shall match only directories use the information
		 provided by the dirent call if possible.  */
	      if ((flags & GLOB_ONLYDIR) && !DIRENT_MIGHT_BE_DIR (d))
		continue;

	      name = d->d_name;

	      if (fnmatch (pattern, name, fnm_flags) == 0)
		{
		  /* If the file we found is a symlink we have to
		     make sure the target file exists.  */
		  if (!DIRENT_MIGHT_BE_SYMLINK (d)
		      || link_exists_p (dfd, directory, dirlen, name, pglob,
					flags))
		    {
		      if (cur == names->count)
			{
			  struct globnames *newnames;
			  size_t count = names->count * 2;
			  size_t size = (sizeof (struct globnames)
					 + ((count - INITIAL_COUNT)
					    * sizeof (char *)));
			  if (__libc_use_alloca (alloca_used + size))
			    newnames = names_alloca
			      = alloca_account (size, alloca_used);
			  else if ((newnames = malloc (size))
				   == NULL)
			    goto memory_error;
			  newnames->count = count;
			  newnames->next = names;
			  names = newnames;
			  cur = 0;
			}
		      names->name[cur] = strdup (d->d_name);
		      if (names->name[cur] == NULL)
			goto memory_error;
		      ++cur;
		      ++nfound;
		    }
		}
	    }
	}
    }

  if (nfound == 0 && (flags & GLOB_NOCHECK))
    {
      size_t len = strlen (pattern);
      nfound = 1;
      names->name[cur] = (char *) malloc (len + 1);
      if (names->name[cur] == NULL)
	goto memory_error;
      *((char *) mempcpy (names->name[cur++], pattern, len)) = '\0';
    }

  int result = GLOB_NOMATCH;
  if (nfound != 0)
    {
      result = 0;

      if (pglob->gl_pathc > UINTPTR_MAX - pglob->gl_offs
	  || pglob->gl_pathc + pglob->gl_offs > UINTPTR_MAX - nfound
	  || pglob->gl_pathc + pglob->gl_offs + nfound > UINTPTR_MAX - 1
	  || (pglob->gl_pathc + pglob->gl_offs + nfound + 1
	      > UINTPTR_MAX / sizeof (char *)))
	goto memory_error;

      char **new_gl_pathv;
      new_gl_pathv
	= (char **) realloc (pglob->gl_pathv,
			     (pglob->gl_pathc + pglob->gl_offs + nfound + 1)
			     * sizeof (char *));
      if (new_gl_pathv == NULL)
	{
	memory_error:
	  while (1)
	    {
	      struct globnames *old = names;
	      for (size_t i = 0; i < cur; ++i)
		free (names->name[i]);
	      names = names->next;
	      /* NB: we will not leak memory here if we exit without
		 freeing the current block assigned to OLD.  At least
		 the very first block is always allocated on the stack
		 and this is the block assigned to OLD here.  */
	      if (names == NULL)
		{
		  assert (old == &init_names);
		  break;
		}
	      cur = names->count;
	      if (old == names_alloca)
		names_alloca = names;
	      else
		free (old);
	    }
	  result = GLOB_NOSPACE;
	}
      else
	{
	  while (1)
	    {
	      struct globnames *old = names;
	      for (size_t i = 0; i < cur; ++i)
		new_gl_pathv[pglob->gl_offs + pglob->gl_pathc++]
		  = names->name[i];
	      names = names->next;
	      /* NB: we will not leak memory here if we exit without
		 freeing the current block assigned to OLD.  At least
		 the very first block is always allocated on the stack
		 and this is the block assigned to OLD here.  */
	      if (names == NULL)
		{
		  assert (old == &init_names);
		  break;
		}
	      cur = names->count;
	      if (old == names_alloca)
		names_alloca = names;
	      else
		free (old);
	    }

	  pglob->gl_pathv = new_gl_pathv;

	  pglob->gl_pathv[pglob->gl_offs + pglob->gl_pathc] = NULL;

	  pglob->gl_flags = flags;
	}
    }

  if (stream != NULL)
    {
      save = errno;
      if (__glibc_unlikely (flags & GLOB_ALTDIRFUNC))
	(*pglob->gl_closedir) (stream);
      else
	closedir (stream);
      __set_errno (save);
    }

  return result;
}