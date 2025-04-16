static inline int compare(const char *s1, const char *s2, int l1, int l2) {
    register const unsigned char *p1 = (const unsigned char *) s1;
    register const unsigned char *p2 = (const unsigned char *) s2;
    register const unsigned char *e1 = p1 + l1, *e2 = p2 + l2;
    int c1, c2;

    while (p1 < e1 && p2 < e2) {
        GET_UTF8_CHAR(p1, e1, c1);
        GET_UTF8_CHAR(p2, e2, c2);
        if (c1 == c2) continue;
        c1 = TOLOWER(c1);
        c2 = TOLOWER(c2);
        if (c1 != c2) return c1 - c2;
    }
    return l1 - l2;
}