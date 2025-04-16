static void check_cell_alloced(pointer p, int expect_alloced)
{
  /* Can't use putstr(sc,str) because callers have no access to
     sc.  */
  if(typeflag(p) & !expect_alloced)
    {
      fprintf(stderr,"Cell is already allocated!\n");
    }
  if(!(typeflag(p)) & expect_alloced)
    {
      fprintf(stderr,"Cell is not allocated!\n");
    }
}