void
Server_warning(Server *self, char * format, ...)
{
#ifndef NO_MESSAGES

    if (self->verbosity & 4)
    {
        char buffer[256];
        va_list args;
        va_start (args, format);
        vsprintf (buffer, format, args);
        va_end (args);
        PySys_WriteStdout("Pyo warning: %s", buffer);
    }

#endif
}