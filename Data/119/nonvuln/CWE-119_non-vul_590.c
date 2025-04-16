unsigned int pdf_print::do_page_resource() 
{
  unsigned int bytes = 0;

  fprintf (stderr, "Page Resource\n");
  new_xref_entry( byte_count /* offset */);
  bytes += pr_out->PutStringC("5 0 obj \n");
  bytes += pr_out->PutStringC("<<  /ProcSet [/PDF] \n");
 // bytes += pr_out->PutStringC("    /Font << /F13 21 0 R >> \n");
  bytes += pr_out->PutStringC(">> \n endobj \n");   
  return(bytes);
}