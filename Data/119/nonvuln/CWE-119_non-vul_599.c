void new_xref_list()
{
  fprintf(stderr, "first xref entry\n");

  xref_root = (xref_entry *) malloc (sizeof (xref_entry));
  xref_root->byte_offset = 0;
  xref_root->generation = 65535;
  xref_root->use = 'f';
  xref_root->next=0;
  xref_count = 1;
}