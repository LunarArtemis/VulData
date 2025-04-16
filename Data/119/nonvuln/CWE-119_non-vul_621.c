pdf_print::~pdf_print()
{
    file_trail();
    if (f_name[0]) {
      if (!nodump)
	pr_out->dump( f_name, Append);
    }
    else
      pr_out->dump( "out.pdf", Append);
}