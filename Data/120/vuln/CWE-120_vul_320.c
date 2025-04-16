void
Server_debug(Server *self, char * format, ...)
{
    if (self->verbosity & 8)
    {
        char buffer[256];
        va_list args;
        va_start (args, format);
        vsprintf (buffer, format, args);
        va_end (args);

        PySys_WriteStdout("Pyo debug: %s", buffer);
    }
}