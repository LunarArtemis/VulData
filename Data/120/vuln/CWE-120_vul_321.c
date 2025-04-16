void
Server_message(Server *self, char * format, ...)
{
    if (self->verbosity & 2)
    {
        char buffer[256];
        va_list args;
        va_start (args, format);
        vsprintf (buffer, format, args);
        va_end (args);

        PySys_WriteStdout("Pyo message: %s", buffer);
    }
}