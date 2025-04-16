static void
write_html_body(
    FILE        *out,			/* I - Output file */
    int         mode,			/* I - HTML or EPUB/XHTML output */
    const char  *bodyfile,		/* I - Body file */
    mmd_t       *body,			/* I - Markdown body */
    mxml_node_t *doc)			/* I - XML documentation */
{
  mxml_node_t	*function,		/* Current function */
		*scut,			/* Struct/class/union/typedef */
		*arg,			/* Current argument */
		*description,		/* Description of function/var */
		*type;			/* Type for argument */
  const char	*name,			/* Name of function/type */
		*defval;		/* Default value */
  bool		whitespace;		/* Current whitespace value */
  const char	*string;		/* Current string value */


 /*
  * Body...
  */

  if (body)
    markdown_write_block(out, body, mode);
  else if (bodyfile)
    write_file(out, bodyfile, mode);

 /*
  * List of classes...
  */

  if ((scut = find_public(doc, doc, "class", NULL, mode)) != NULL)
  {
    fputs("<h2 class=\"title\"><a id=\"CLASSES\">Classes</a></h2>\n", out);

    while (scut)
    {
      write_scu(out, mode, doc, scut);

      scut = find_public(scut, doc, "class", NULL, mode);
    }
  }

 /*
  * List of functions...
  */

  if ((function = find_public(doc, doc, "function", NULL, mode)) != NULL)
  {
    fputs("<h2 class=\"title\"><a id=\"FUNCTIONS\">Functions</a></h2>\n", out);

    while (function)
    {
      write_function(out, mode, doc, function, 3);

      function = find_public(function, doc, "function", NULL, mode);
    }
  }

 /*
  * List of types...
  */

  if ((scut = find_public(doc, doc, "typedef", NULL, mode)) != NULL)
  {
    fputs("<h2 class=\"title\"><a id=\"TYPES\">Data Types</a></h2>\n", out);

    while (scut)
    {
      name        = mxmlElementGetAttr(scut, "name");
      description = mxmlFindElement(scut, scut, "description", NULL, NULL, MXML_DESCEND_FIRST);
      fprintf(out, "<h3 class=\"typedef\"><a id=\"%s\">%s%s</a></h3>\n", name, get_comment_info(description), name);

      if (description)
	write_description(out, mode, description, "p", 1);

      fputs("<p class=\"code\">\n"
	    "typedef ", out);

      type = mxmlFindElement(scut, scut, "type", NULL, NULL, MXML_DESCEND_FIRST);

      for (type = mxmlGetFirstChild(type); type; type = mxmlGetNextSibling(type))
      {
        string = mxmlGetText(type, &whitespace);

        if (!strcmp(string, "("))
        {
	  break;
	}
	else
	{
	  if (whitespace)
	    putc(' ', out);

	  if (find_public(doc, doc, "class", string, mode) || find_public(doc, doc, "enumeration", string, mode) || find_public(doc, doc, "struct", string, mode) || find_public(doc, doc, "typedef", string, mode) || find_public(doc, doc, "union", string, mode))
	  {
            fputs("<a href=\"#", out);
            write_string(out, string, OUTPUT_HTML, 0);
	    fputs("\">", out);
            write_string(out, string, OUTPUT_HTML, 0);
	    fputs("</a>", out);
	  }
	  else
            write_string(out, string, OUTPUT_HTML, 0);
        }
      }

      if (type)
      {
       /*
        * Output function type...
	*/

        string = mxmlGetText(mxmlGetPrevSibling(type), NULL);

        if (string && *string != '*')
	  putc(' ', out);

        fprintf(out, "(*%s", name);

	for (type = mxmlGetNextSibling(mxmlGetNextSibling(type)); type; type = mxmlGetNextSibling(type))
	{
	  string = mxmlGetText(type, &whitespace);

	  if (whitespace)
	    putc(' ', out);

	  if (find_public(doc, doc, "class", string, mode) || find_public(doc, doc, "enumeration", string, mode) || find_public(doc, doc, "struct", string, mode) || find_public(doc, doc, "typedef", string, mode) || find_public(doc, doc, "union", string, mode))
	  {
            fputs("<a href=\"#", out);
            write_string(out, string, OUTPUT_HTML, 0);
	    fputs("\">", out);
            write_string(out, string, OUTPUT_HTML, 0);
	    fputs("</a>", out);
	  }
	  else
            write_string(out, string, OUTPUT_HTML, 0);
        }

        fputs(";\n", out);
      }
      else
      {
	type   = mxmlFindElement(scut, scut, "type", NULL, NULL, MXML_DESCEND_FIRST);
	string = mxmlGetText(mxmlGetLastChild(type), NULL);

        if (*string != '*')
	  putc(' ', out);

	fprintf(out, "%s;\n", name);
      }

      fputs("</p>\n", out);

      scut = find_public(scut, doc, "typedef", NULL, mode);
    }
  }

 /*
  * List of structures...
  */

  if ((scut = find_public(doc, doc, "struct", NULL, mode)) != NULL)
  {
    fputs("<h2 class=\"title\"><a id=\"STRUCTURES\">Structures</a></h2>\n", out);

    while (scut)
    {
      write_scu(out, mode, doc, scut);

      scut = find_public(scut, doc, "struct", NULL, mode);
    }
  }

 /*
  * List of unions...
  */

  if ((scut = find_public(doc, doc, "union", NULL, mode)) != NULL)
  {
    fputs("<h2 class=\"title\"><a id=\"UNIONS\">Unions</a></h2>\n", out);

    while (scut)
    {
      write_scu(out, mode, doc, scut);

      scut = find_public(scut, doc, "union", NULL, mode);
    }
  }

 /*
  * Variables...
  */

  if ((arg = find_public(doc, doc, "variable", NULL, mode)) != NULL)
  {
    fputs("<h2 class=\"title\"><a id=\"VARIABLES\">Variables</a></h2>\n", out);

    while (arg)
    {
      name        = mxmlElementGetAttr(arg, "name");
      description = mxmlFindElement(arg, arg, "description", NULL, NULL, MXML_DESCEND_FIRST);
      fprintf(out, "<h3 class=\"variable\"><a id=\"%s\">%s%s</a></h3>\n", name, get_comment_info(description), name);

      if (description)
	write_description(out, mode, description, "p", 1);

      fputs("<p class=\"code\">", out);

      write_element(out, doc, mxmlFindElement(arg, arg, "type", NULL, NULL, MXML_DESCEND_FIRST), OUTPUT_HTML);
      fputs(mxmlElementGetAttr(arg, "name"), out);
      if ((defval = mxmlElementGetAttr(arg, "default")) != NULL)
	fprintf(out, " %s", defval);
      fputs(";</p>\n", out);

      arg = find_public(arg, doc, "variable", NULL, mode);
    }
  }

 /*
  * List of enumerations...
  */

  if ((scut = find_public(doc, doc, "enumeration", NULL, mode)) != NULL)
  {
    fputs("<h2 class=\"title\"><a id=\"ENUMERATIONS\">Constants</a></h2>\n", out);

    while (scut)
    {
      name        = mxmlElementGetAttr(scut, "name");
      description = mxmlFindElement(scut, scut, "description", NULL, NULL, MXML_DESCEND_FIRST);
      fprintf(out, "<h3 class=\"enumeration\"><a id=\"%s\">%s%s</a></h3>\n", name, get_comment_info(description), name);

      if (description)
	write_description(out, mode, description, "p", 1);

      fputs("<h4 class=\"constants\">Constants</h4>\n"
            "<table class=\"list\"><tbody>\n", out);

      for (arg = find_public(scut, scut, "constant", NULL, mode); arg; arg = find_public(arg, scut, "constant", NULL, mode))
      {
	description = mxmlFindElement(arg, arg, "description", NULL, NULL, MXML_DESCEND_FIRST);
	fprintf(out, "<tr><th>%s %s</th>", mxmlElementGetAttr(arg, "name"), get_comment_info(description));

	write_description(out, mode, description, "td", -1);
        fputs("</tr>\n", out);
      }

      fputs("</tbody></table>\n", out);

      scut = find_public(scut, doc, "enumeration", NULL, mode);
    }
  }
}