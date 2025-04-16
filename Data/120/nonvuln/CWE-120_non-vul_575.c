static inline char *str_append(char *pos, const char *value, size_t len) {
    memcpy(pos, value, len);
    return pos + len;
}