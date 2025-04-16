char *
XkbVModMaskText(XkbDescPtr xkb,
                unsigned modMask, unsigned mask, unsigned format)
{
    register int i, bit;
    int len;
    char *mm, *rtrn;
    char *str, buf[BUFFER_SIZE];

    if ((modMask == 0) && (mask == 0)) {
        rtrn = tbGetBuffer(5);
        if (format == XkbCFile)
            sprintf(rtrn, "0");
        else
            sprintf(rtrn, "none");
        return rtrn;
    }
    if (modMask != 0)
        mm = XkbModMaskText(modMask, format);
    else
        mm = NULL;

    str = buf;
    buf[0] = '\0';
    if (mask) {
        char *tmp;

        for (i = 0, bit = 1; i < XkbNumVirtualMods; i++, bit <<= 1) {
            if (mask & bit) {
                tmp = XkbVModIndexText(xkb, i, format);
                len = strlen(tmp) + 1 + (str == buf ? 0 : 1);
                if (format == XkbCFile)
                    len += 4;
                if ((str - (buf + len)) <= BUFFER_SIZE) {
                    if (str != buf) {
                        if (format == XkbCFile)
                            *str++ = '|';
                        else
                            *str++ = '+';
                        len--;
                    }
                }
                if (format == XkbCFile)
                    sprintf(str, "%sMask", tmp);
                else
                    strcpy(str, tmp);
                str = &str[len - 1];
            }
        }
        str = buf;
    }
    else
        str = NULL;
    if (mm)
        len = strlen(mm);
    else
        len = 0;
    if (str)
        len += strlen(str) + (mm == NULL ? 0 : 1);
    if (len >= BUFFER_SIZE)
        len = BUFFER_SIZE - 1;
    rtrn = tbGetBuffer(len + 1);
    rtrn[0] = '\0';

    if (mm != NULL) {
        i = strlen(mm);
        if (i > len)
            i = len;
        strcpy(rtrn, mm);
    }
    else {
        i = 0;
    }
    if (str != NULL) {
        if (mm != NULL) {
            if (format == XkbCFile)
                strcat(rtrn, "|");
            else
                strcat(rtrn, "+");
        }
        strncat(rtrn, str, len - i);
    }
    rtrn[len] = '\0';
    return rtrn;
}