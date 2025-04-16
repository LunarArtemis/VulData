unsigned int pdf_print::do_page_leaf() 
{
  unsigned int bytes = 0;

  fprintf (stderr, "Page Leaf\n");
  new_xref_entry( byte_count /* offset */);
  bytes += pr_out->PutStringC("3 0 obj\n");
  bytes += pr_out->PutStringC("  << /Type /Page\n");
  bytes += pr_out->PutStringC("     /Parent 2 0 R\n");
  bytes += pr_out->PutStringC("     /MediaBox [0 0 621 792]\n");
  //bytes += pr_out->PutStringC("     /Contents 4 0 R\n");
  bytes += pr_out->PutStringC("     /Contents 6 0 R\n");
  bytes += pr_out->PutStringC("     /ProcSet 5 0 R\n");
  bytes += pr_out->PutStringC("     /Resources << /Font  << /F13 21 0 R >> >> \n");
  bytes += pr_out->PutStringC("  >>\nendobj\n"); 
  return (bytes);
}