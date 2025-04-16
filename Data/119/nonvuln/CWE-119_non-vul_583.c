int pdf_print::do_page(i_buf *i_b,  struct font_list *f_a[])
{
  npages++;
  byte_count += do_page_content(i_b, f_a);
  //  format_page(this, i_b, f_a, f_i);
  return (page_retval);
}