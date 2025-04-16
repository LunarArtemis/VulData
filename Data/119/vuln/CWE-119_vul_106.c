char *
XkbAtomText(Atom atm, unsigned format)
{
    const char *atmstr;
    char *rtrn, *tmp;

    atmstr = NameForAtom(atm);
    if (atmstr != NULL) {
        int len;

        len = strlen(atmstr) + 1;
        if (len > BUFFER_SIZE)
            len = BUFFER_SIZE - 2;
        rtrn = tbGetBuffer(len);
        strlcpy(rtrn, atmstr, len);
    }
    else {
        rtrn = tbGetBuffer(1);
        rtrn[0] = '\0';
    }
    if (format == XkbCFile) {
        for (tmp = rtrn; *tmp != '\0'; tmp++) {
            if ((tmp == rtrn) && (!isalpha(*tmp)))
                *tmp = '_';
            else if (!isalnum(*tmp))
                *tmp = '_';
        }
    }
    return XkbStringText(rtrn, format);
}