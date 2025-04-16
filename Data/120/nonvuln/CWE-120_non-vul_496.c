static int utf8_stricmp(const char *s1, const char *s2)
{
  char *s1a, *s2a;
  int result;

  s1a = g_utf8_casefold(s1, -1);
  s2a = g_utf8_casefold(s2, -1);

  result = g_utf8_collate(s1a, s2a);

  g_free(s1a);
  g_free(s2a);
  return result;
}