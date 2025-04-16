int ossl_a2ucompare(const char *a, const char *u)
{
    char a_ulabel[LABEL_BUF_SIZE];
    size_t a_size = sizeof(a_ulabel);

    if (ossl_a2ulabel(a, a_ulabel, &a_size) <= 0) {
        return -1;
    }

    return (strcmp(a_ulabel, u) == 0) ? 0 : 1;
}