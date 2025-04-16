void
Server_error(Server *self, char * format, ...)
{
    if (self->verbosity & 1)
    {
        char buffer[256];
        va_list args;
        va_start (args, format);
        vsprintf (buffer, format, args);
        va_end (args);

        PySys_WriteStdout("Pyo error: %s", buffer);
    }
}