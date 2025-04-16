void pdf_print::page_head()
{
  byte_count += do_page_leaf();

  init_hv();
}