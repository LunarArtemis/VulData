static PyObject *
PyCArg_repr(PyCArgObject *self)
{
    char buffer[256];
    switch(self->tag) {
    case 'b':
    case 'B':
        sprintf(buffer, "<cparam '%c' (%d)>",
            self->tag, self->value.b);
        break;
    case 'h':
    case 'H':
        sprintf(buffer, "<cparam '%c' (%d)>",
            self->tag, self->value.h);
        break;
    case 'i':
    case 'I':
        sprintf(buffer, "<cparam '%c' (%d)>",
            self->tag, self->value.i);
        break;
    case 'l':
    case 'L':
        sprintf(buffer, "<cparam '%c' (%ld)>",
            self->tag, self->value.l);
        break;

    case 'q':
    case 'Q':
        sprintf(buffer,
#ifdef MS_WIN32
            "<cparam '%c' (%I64d)>",
#else
            "<cparam '%c' (%lld)>",
#endif
            self->tag, self->value.q);
        break;
    case 'd':
        sprintf(buffer, "<cparam '%c' (%f)>",
            self->tag, self->value.d);
        break;
    case 'f':
        sprintf(buffer, "<cparam '%c' (%f)>",
            self->tag, self->value.f);
        break;

    case 'c':
        if (is_literal_char((unsigned char)self->value.c)) {
            sprintf(buffer, "<cparam '%c' ('%c')>",
                self->tag, self->value.c);
        }
        else {
            sprintf(buffer, "<cparam '%c' ('\\x%02x')>",
                self->tag, (unsigned char)self->value.c);
        }
        break;

/* Hm, are these 'z' and 'Z' codes useful at all?
   Shouldn't they be replaced by the functionality of c_string
   and c_wstring ?
*/
    case 'z':
    case 'Z':
    case 'P':
        sprintf(buffer, "<cparam '%c' (%p)>",
            self->tag, self->value.p);
        break;

    default:
        if (is_literal_char((unsigned char)self->tag)) {
            sprintf(buffer, "<cparam '%c' at %p>",
                (unsigned char)self->tag, (void *)self);
        }
        else {
            sprintf(buffer, "<cparam 0x%02x at %p>",
                (unsigned char)self->tag, (void *)self);
        }
        break;
    }
    return PyUnicode_FromString(buffer);
}