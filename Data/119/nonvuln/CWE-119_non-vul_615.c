pdf_print::pdf_print(font_list *font_array[], file_info *f)
{
    int j;

    //    printf ("val %d\n", inch_to_dvi(0.4218));
    //    printf ("val %d\n", mm_to_dvi(17.6));
    //    printf ("val %f\n", dvi_to_inch(98670000));
    npages=0;
    byte_count = 0;
    xref_offset = 0;
    generation = 0;
    memset (pdf_stream_b, 0, sizeof(pdf_stream_b));
    new_xref_list();

    if (f->m_flags & A4 ) { // a4 is  297mm*210mm
      if (!(f->flags & ROTATE)) 
	pdf_top_of_page = 98670000 +  6563672/* 6712847 */;
      else  
	pdf_top_of_page = 98670000;
    }
    else
      pdf_top_of_page = 98670000;
    /*    printf("top of page %f %f %u %u\n",  
	   dvi_to_inch(pdf_top_of_page),
	   dvi_to_mm(pdf_top_of_page),
	   pdf_top_of_page,
	   mm_to_dvi(32.42)); */
    for (int i=0; i< 256; i++) pdf_used[i] = 0;
    f_i = f;
    f_a = font_array;
    f_name[0] = '\0';
    for ( j=0; j<FONT_NAMES; j++)
      font_sizes[j] = f->font_sizes[j];
    if (*f->out_file) {
      strcat(f_name, f->out_file);
    }
    highlight_type=Gray;
    if (f->m_flags & SOUND)
      nodump = 1;
    else nodump = 0;

    file_head();		// file head dumps header
}