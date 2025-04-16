unsigned int pdf_print::do_catalog()
{
  unsigned int bytes = 0;

  fprintf (stderr, "Catalog\n");
  /* save byte position of first byte */
  new_xref_entry( byte_count /* offset */);
  bytes += pr_out->PutStringC("1 0 obj\n");
  bytes += pr_out->PutStringC("  << /Type /Catalog\n");
  bytes += pr_out->PutStringC("     /Pages 2 0 R\n");
  bytes += pr_out->PutStringC("  >>\nendobj\n"); 
  return (bytes);
}