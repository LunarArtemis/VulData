int am_read_post_data(request_rec *r, char **data, apr_size_t *length)
{
    apr_size_t bytes_read;
    apr_size_t bytes_left;
    apr_size_t len;
    long read_length;
    int rc;

    /* Prepare to receive data from the client. We request that apache
     * dechunks data if it is chunked.
     */
    rc = ap_setup_client_block(r, REQUEST_CHUNKED_DECHUNK);
    if (rc != OK) {
        return rc;
    }

    /* This function will send a 100 Continue response if the client is
     * waiting for that. If the client isn't going to send data, then this
     * function will return 0.
     */
    if (!ap_should_client_block(r)) {
        len = 0;
    } else {
        len = r->remaining;
    }

    if (length != NULL) {
        *length = len;
    }

    *data = (char *)apr_palloc(r->pool, len + 1);

    /* Make sure that the data is null-terminated.  */
    (*data)[len] = '\0';

    bytes_read = 0;
    bytes_left = len;

    while (bytes_left > 0) {
        /* Read data from the client. Returns 0 on EOF or error, the
         * number of bytes otherwise.
         */
        read_length = ap_get_client_block(r, &(*data)[bytes_read],
                                          bytes_left);
        if (read_length == 0) {
            ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, r,
                          "Failed to read POST data from client.");
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        bytes_read += read_length;
        bytes_left -= read_length;
    }

    return OK;
}