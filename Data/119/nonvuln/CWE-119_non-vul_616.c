void new_xref_entry(const int offset)
{
  xref_entry *t, *u;

  fprintf(stderr, "adding xref entry offset %u\n", offset);

  t = (xref_entry *) malloc (sizeof (xref_entry));
  if ( ! t ) { fprintf(stderr, "new_xref_entry: malloc failed\n");}

  t->byte_offset = offset;
  t->generation = 0;
  t->use = 'n';
  t->next = 0;

  xref_count += 1;
  
  u = xref_root;
  
  while (u->next) {
    //    fprintf(stderr, "increment xref list\n");
    u = u->next;
  }
  u->next = t;

}