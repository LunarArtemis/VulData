static void check_range_alloced(pointer p, int n, int expect_alloced)
{
  int i;
  for(i = 0;i<n;i++)
    { (void)check_cell_alloced(p+i,expect_alloced); }
}