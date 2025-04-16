DltReturnValue dlt_file_message(DltFile *file, int index, int verbose)
{
    PRINT_FUNCTION_VERBOSE(verbose);

    if (file == NULL)
        return DLT_RETURN_WRONG_PARAMETER;

    /* check if message is in range */
    if (index >= file->counter) {
        dlt_vlog(LOG_WARNING, "Message %d out of range!\r\n", index);
        return DLT_RETURN_WRONG_PARAMETER;
    }

    /* seek to position in file */
    if (fseek(file->handle, file->index[index], SEEK_SET) != 0) {
        dlt_vlog(LOG_WARNING, "Seek to message %d to position %ld failed!\r\n",
                 index, file->index[index]);
        return DLT_RETURN_ERROR;
    }

    /* read all header and payload */
    if (dlt_file_read_header(file, verbose) < DLT_RETURN_OK)
        return DLT_RETURN_ERROR;

    if (dlt_file_read_header_extended(file, verbose) < DLT_RETURN_OK)
        return DLT_RETURN_ERROR;

    if (dlt_file_read_data(file, verbose) < DLT_RETURN_OK)
        return DLT_RETURN_ERROR;

    /* set current position in file */
    file->position = index;

    return DLT_RETURN_OK;
}