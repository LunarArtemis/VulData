static INLINE int is_one_of(char *s, gunichar c) {
  if (c==EOF)
     return 1;

  if (g_utf8_strchr(s, -1, c) != NULL)
     return (1);

  return (0);
}