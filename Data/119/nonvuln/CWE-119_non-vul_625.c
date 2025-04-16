unsigned int pdf_print::do_rule_stream(i_buf *i_b,  struct font_list *f_a[],
				  char *s_buf) {
  unsigned int bytes = 0;

  fprintf(stderr,"pdf_print: do_rule_stream: bytes dec %d oct %o\n",
	  bytes, bytes);
 
  //  pdf_print *page_buf;
  
  page_retval = (format_page(this, i_b, f_a, f_i));
  
  strcat (s_buf,  pdf_stream_b );
  
  bytes = strlen(s_buf);
  printf("do_rule_stream: bytes %u \n", bytes);
  if (bytes > STREAM) { dbg1(Error, 
     "tab: pdf_print.cc: do_rule_stream: bytes greater than STREAM %d\n",
			     (void *)bytes); }
  return (bytes);
}