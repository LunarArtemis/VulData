static void
_dirnameW(WCHAR *path)
{
    WCHAR *ptr;

    /* walk the path from the end until a backslash is encountered */
    for(ptr = path + wcslen(path); ptr != path; ptr--) {
        if (*ptr == L'\\' || *ptr == L'/')
            break;
    }
    *ptr = 0;
}