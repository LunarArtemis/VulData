void pdf_print::file_trail()		// write_postamble
{
  char buf[80];
  //   flush();

  file_xref();

  pr_out->PutString("trailer\n");
  sprintf (buf, "<< /Size %d\n", xref_count );
  pr_out->PutString(buf);
  pr_out->PutString("   /Root 1 0 R\n");
  pr_out->PutString(">>\n");

  pr_out->PutString("startxref\n");
  pr_out->Put10(byte_count);
  pr_out->PutString("\n");
  pr_out->PutString("%%EOF\n"); //  official last line!
}