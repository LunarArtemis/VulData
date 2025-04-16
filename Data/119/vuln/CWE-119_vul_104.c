char *
XkbVModIndexText(XkbDescPtr xkb, unsigned ndx, unsigned format)
{
    register int len;
    register Atom *vmodNames;
    char *rtrn;
    const char *tmp;
    char numBuf[20];

    if (xkb && xkb->names)
        vmodNames = xkb->names->vmods;
    else
        vmodNames = NULL;

    tmp = NULL;
    if (ndx >= XkbNumVirtualMods)
        tmp = "illegal";
    else if (vmodNames && (vmodNames[ndx] != None))
        tmp = NameForAtom(vmodNames[ndx]);
    if (tmp == NULL) {
        snprintf(numBuf, sizeof(numBuf), "%d", ndx);
        tmp = numBuf;
    }

    len = strlen(tmp) + 1;
    if (format == XkbCFile)
        len += 4;
    if (len >= BUFFER_SIZE)
        len = BUFFER_SIZE - 1;
    rtrn = tbGetBuffer(len);
    if (format == XkbCFile) {
        strcpy(rtrn, "vmod_");
        strncpy(&rtrn[5], tmp, len - 4);
    }
    else
        strncpy(rtrn, tmp, len);
    return rtrn;
}