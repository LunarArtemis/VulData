static void
highlight_c_string(FILE       *fp,	/* I  - Output file */
                   const char *s,	/* I  - String */
                   int        *histate)	/* IO - Highlighting state */
{
  const char	*start = s,		/* Start of code to highlight */
		*class_name = (*histate == HIGHLIGHT_COMMENT) ? "comment" : NULL;
					/* Class name for current fragment */
  char		keyword[32],		/* Current keyword */
		*keyptr = keyword;	/* Pointer into keyword */


  if (*histate == HIGHLIGHT_COMMENT)
  {
    if ((s = strstr(start, "*/")) != NULL)
    {
     /*
      * Comment ends on this line...
      */

      s += 2;

      fputs("<span class=\"comment\">", fp);
      write_string(fp, start, OUTPUT_HTML, s - start);
      fputs("</span>", fp);

      start      = s;
      *histate   = HIGHLIGHT_NONE;
      class_name = NULL;
    }
    else
    {
     /*
      * Comment continues beyond the current line...
      */

      s = start + strlen(start) - 1;
    }
  }
  else if (*s == '#')
  {
   /*
    * Preprocessor directive...
    */

    while (*s && *s != '\n')
    {
      if (!strncmp(s, "/*", 2) || !strncmp(s, "//", 2))
        break;

      s ++;
    }

    fputs("<span class=\"directive\">", fp);
    write_string(fp, start, OUTPUT_HTML, s - start);
    fputs("</span>", fp);

    start = s;
  }

  while (*s && *s != '\n')
  {
    if (!strncmp(s, "/*", 2))
    {
     /*
      * Start of a block comment...
      */

      if (s > start)
      {
       /*
        * Output current fragment...
        */

	if (class_name)
	{
	  fprintf(fp, "<span class=\"%s\">", class_name);
	  write_string(fp, start, OUTPUT_HTML, s - start);
	  fputs("</span>", fp);
	}
	else
	{
	  write_string(fp, start, OUTPUT_HTML, s - start);
	}

	start = s;
      }

     /*
      * At this point, "start" points to the start of the comment...
      */

      if ((s = strstr(start, "*/")) != NULL)
      {
       /*
        * Comment ends on the current line...
        */

        s += 2;

	fputs("<span class=\"comment\">", fp);
	write_string(fp, start, OUTPUT_HTML, s - start);
	fputs("</span>", fp);

	start      = s;
	*histate   = HIGHLIGHT_NONE;
	class_name = NULL;
      }
      else
      {
       /*
        * Comment continues to the next line...
        */

	s          = start + strlen(start) - 1;
	*histate   = HIGHLIGHT_COMMENT;
	class_name = "comment";
	break;
      }
    }
    else if (!strncmp(s, "//", 2))
    {
     /*
      * Start of C++ comment...
      */

      if (s > start)
      {
       /*
        * Output current fragment...
        */

	if (class_name)
	{
	  fprintf(fp, "<span class=\"%s\">", class_name);
	  write_string(fp, start, OUTPUT_HTML, s - start);
	  fputs("</span>", fp);
	}
	else
	{
	  write_string(fp, start, OUTPUT_HTML, s - start);
	}

	start = s;
      }

      s          = start + strlen(start) - 1;
      *histate   = HIGHLIGHT_COMMENT1;
      class_name = "comment";
      break;
    }
    else if (*s == '\"' || *s == '\'')
    {
     /*
      * String/character constant...
      */

      if (s > start)
      {
       /*
        * Output current fragment...
        */

	if (class_name)
	{
	  fprintf(fp, "<span class=\"%s\">", class_name);
	  write_string(fp, start, OUTPUT_HTML, s - start);
	  fputs("</span>", fp);
	}
	else
	{
	  write_string(fp, start, OUTPUT_HTML, s - start);
	}

	start = s;
      }

      for (s = start + 1; *s && *s != *start; s ++)
      {
        if (*s == '\\' && s[1])
          s ++;
      }

      if (*s == *start)
        s ++;

      fputs("<span class=\"string\">", fp);
      write_string(fp, start, OUTPUT_HTML, s - start);
      fputs("</span>", fp);

      start = s;
    }
    else
    {
      if (isalnum(*s & 255) || *s == '_' || *s == '.')
      {
       /*
        * Number or keyword...
        */

	if (*histate == HIGHLIGHT_NONE)
	{
	  if (s > start && *histate == HIGHLIGHT_NONE)
	  {
	   /*
	    * End current fragment...
	    */

	    write_string(fp, start, OUTPUT_HTML, s - start);
	    start = s;
	  }

	  if (isdigit(*s & 255) || (*s == '.' && isdigit(s[1] & 255)))
	  {
	    *histate   = HIGHLIGHT_NUMBER;
	    class_name = "number";
	  }
	  else
	  {
	    *histate = HIGHLIGHT_RESERVED;
	  }
	}

        if (*histate == HIGHLIGHT_RESERVED && keyptr < (keyword + sizeof(keyword) - 1))
          *keyptr++ = *s;
      }
      else if (*histate == HIGHLIGHT_NUMBER)
      {
       /*
        * End of number...
        */

	fprintf(fp, "<span class=\"%s\">", class_name);
	write_string(fp, start, OUTPUT_HTML, s - start);
	fputs("</span>", fp);

	start      = s;
	*histate   = HIGHLIGHT_NONE;
	class_name = NULL;
      }
      else if (*histate == HIGHLIGHT_RESERVED)
      {
       /*
        * End of reserved word?
        */

        *keyptr = '\0';
        keyptr  = keyword;

        if (is_reserved(keyword))
        {
         /*
          * Yes, reserved word...
          */

	  fputs("<span class=\"reserved\">", fp);
	  write_string(fp, start, OUTPUT_HTML, s - start);
	  fputs("</span>", fp);
        }
        else
        {
         /*
          * Just some other text...
          */

	  write_string(fp, start, OUTPUT_HTML, s - start);
        }

        start    = s;
        *histate = HIGHLIGHT_NONE;
      }

      s ++;
    }
  }

  if (s > start)
  {
    if (class_name)
    {
      fprintf(fp, "<span class=\"%s\">", class_name);
      write_string(fp, start, OUTPUT_HTML, s - start);
      fputs("</span>", fp);
    }
    else if (*histate == HIGHLIGHT_RESERVED)
    {
      *keyptr = '\0';
      keyptr  = keyword;

      if (is_reserved(keyword))
      {
       /*
	* Yes, reserved word...
	*/

	fputs("<span class=\"reserved\">", fp);
	write_string(fp, start, OUTPUT_HTML, s - start);
	fputs("</span>", fp);
      }
      else
      {
       /*
	* Just some other text...
	*/

	write_string(fp, start, OUTPUT_HTML, s - start);
      }
    }
    else
    {
      write_string(fp, start, OUTPUT_HTML, s - start);
    }
  }

  if (*histate != HIGHLIGHT_COMMENT)
    *histate = HIGHLIGHT_NONE;

  putc('\n', fp);
}