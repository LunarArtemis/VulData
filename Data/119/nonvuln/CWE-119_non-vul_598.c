unsigned int pdf_print::do_page_content(i_buf *i_b,  struct font_list *f_a[])
{
  unsigned int bytes = 0;
  char b[128];
  unsigned int scount ;
  char s_buf[STREAM];

  memset (s_buf, 0, sizeof(s_buf));
  fprintf (stderr, "Page Content\n");
  new_xref_entry( byte_count /* offset */);
  bytes += pr_out->PutStringC("4 0 obj\n");
  scount = do_rule_stream(i_b,  f_a, s_buf);

  // we need to know byte count of content stream here
  // count includes trailing newline
    
  sprintf (b, "<</Length %u>>\n", scount);
  bytes += pr_out->PutStringC(b);

  bytes += pr_out->PutStringC("stream\n");
  
  bytes += pr_out->PutStringC(s_buf);
  //  print_stream();
  // we go byte count in scount above
  bytes += pr_out->PutStringC("endstream\n");
  bytes += pr_out->PutStringC("endobj\n");

  bytes += do_text_stream(i_b, f_a, s_buf);
  return(bytes);
}