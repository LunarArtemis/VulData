static int ucmp(const void *_s1, const void *_s2) {
return( u_strcmp(*(const unichar_t **)_s1,*(const unichar_t **)_s2));
}