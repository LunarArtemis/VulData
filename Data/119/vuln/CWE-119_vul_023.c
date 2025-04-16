void pdf_print::file_xref()
{
  xref_entry *u;
  char tstring[20];

  pr_out->PutString("xref\n");
  sprintf (tstring, "%d %d\n", 0, xref_count);
  pr_out->PutString(tstring);

  u = xref_root;
  if (! u ) {
    dbg0 (Error, "pdf_print: file_xref: uninitialized list");
  }

  while (u) {
    sprintf (tstring, "%010d %05d %c \n", u->byte_offset, u->generation, u->use);
    pr_out->PutString(tstring);
    u = u->next;
    // free nodes here
  }
}