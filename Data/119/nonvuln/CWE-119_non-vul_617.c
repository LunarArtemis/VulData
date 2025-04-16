unsigned int pdf_print::do_text_stream(i_buf *i_b,  struct font_list *f_a[],
				  char *s_buf) {
  unsigned int bytes = 0;
  unsigned int scount = 0;
  char b[16];
  
  fprintf(stderr,"pdf_print: do_text_stream: bytes dec %d oct %o\n",
	  bytes, bytes);
  
  memset (s_buf, 0, sizeof(s_buf));
  strcpy (s_buf, "BT\n/F13 12 Tf\n 288 520 Tda\n (ABC) Tj\nET \n");
  scount = strlen(s_buf);
  bytes += pr_out->PutStringC("6 0 obj\n<</Length ");
  sprintf (b, "%d", scount);
  bytes += pr_out->PutStringC(b);
  bytes += pr_out->PutStringC(">>\nstream\n");
  bytes += pr_out->PutStringC(s_buf);
  bytes += pr_out->PutStringC("endstream\nendobj\n");
  return (bytes);
}