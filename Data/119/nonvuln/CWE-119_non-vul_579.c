void pdf_print::page_trail()
{
  //  byte_count += do_page_content();
  byte_count += do_page_resource();
// why? 	reset_dvi_vh();
//    flush();
}