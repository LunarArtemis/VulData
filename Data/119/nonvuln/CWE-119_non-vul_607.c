void pdf_print::file_head()
{
    i_buf pdf_header;
    // time_t t;
    char pk_name[300];
    char *p = NULL;

    if (nodump) 
      return;

    byte_count =  pdf_header.PutStringC("%PDF-1.4\n");
    byte_count += pdf_header.PutStringC("%’");
    byte_count += pdf_header.PutStringC("\n");
    /* byte_count += 1; */ /* wbc april test */
    fprintf(stderr, "file_head: byte count is %d\n", byte_count);
    byte_count += do_catalog();
    fprintf(stderr, "file_head: byte count is %d\n", byte_count);
    byte_count += do_page_tree();
    fprintf(stderr, "file_head: byte count is %d\n", byte_count);
    //    byte_count += do_page_leaf();
    //    byte_count += do_page_content();
    //    byte_count += do_page_resource();
    byte_count += pdf_fontdef();

    if (font_path) {
      //fprintf (stderr, "pdf_print - setting font path %s from command line\n", 
      //font_path);
      p = font_path;
    }
    else
      p = getenv("TABFONTS");
    if (p == NULL ) 
#ifdef TFM_PATH
	strcpy(pk_name, TFM_PATH);
#else
    strcpy(pk_name, ".");
#endif /* TFM_PATH */
    else
	strcpy(pk_name, (char *)p);

    /* free(p); windows */
    strcat (pk_name, "/");

    if (f_i->font_names[0]) {
      strcat (pk_name, f_i->font_names[0]);
    }
    else {
      if (baroque) {
	strcat (pk_name, "blute");
      }
      else
	strcat (pk_name, "lute");
    }
    if (red == 1.00)
	strcat (pk_name, "9");
    else if (red == 0.94440)
	strcat (pk_name, "85");
    else if (red == 0.88880)
	strcat (pk_name, "8");
    else if (red == 0.777770)
	strcat (pk_name, "7");
    else 
	strcat (pk_name, "6");
    
    if (f_i->m_flags & DPI1200)
	strcat (pk_name, ".1200pk");
    else if (f_i->m_flags & DPI2400)
	strcat (pk_name, ".2400pk");
    else if (f_i->flags & DPI600)
	strcat (pk_name, ".600pk");
    else
	strcat (pk_name, ".300pk");

    file_in pk_in(pk_name, "rb");

    read_pk_file(&pk_in, this);

    //    make_pdf_font(&pdf_header);

    //    pdf_header.PutString("%%EndSetup\n");

    if (f_name[0]) {
	pdf_header.dump( f_name, Creat);
    }
    else
	pdf_header.dump("out.pdf", Creat);
}