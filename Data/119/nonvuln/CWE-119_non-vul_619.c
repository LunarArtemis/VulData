unsigned int pdf_print::do_page_tree() 
{
  unsigned int bytes = 0;

  fprintf (stderr, "Page Tree\n");
  new_xref_entry( byte_count /* offset */);
  bytes += pr_out->PutStringC("2 0 obj\n");
  bytes += pr_out->PutStringC("  << /Type /Pages\n");
  bytes += pr_out->PutStringC("     /Kids [3 0 R]\n");
  bytes += pr_out->PutStringC("     /Count 1\n");
  bytes += pr_out->PutStringC("  >>\nendobj\n"); 
  return (bytes);
}