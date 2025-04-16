int my_pdf_isprint(int c)
{
  if (c < 33 ) return (0);
  if (c > 126 ) return (0);
  return (1);
}