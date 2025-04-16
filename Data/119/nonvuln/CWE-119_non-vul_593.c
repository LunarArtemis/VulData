unsigned int pdf_print::pdf_fontdef()
{
  unsigned int bytes = 0;
   fprintf (stderr, "Define Font\n");
   new_xref_entry( byte_count /* offset */);
   bytes += pr_out->PutStringC("21 0 obj\n");
   bytes += pr_out->PutStringC("<<  /Type /Font \n   /Suntype /Type1 \n   /BaseFont /Helvetica \n >> \n");
   bytes += pr_out->PutStringC("endobj\n"); 
  return(bytes);
}