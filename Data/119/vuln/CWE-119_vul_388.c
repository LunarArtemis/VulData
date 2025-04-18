static gboolean
iseries_check_file_type (wtap * wth, int *err, gchar **err_info, int format)
{
  guint      line;
  int        num_items_scanned;
  char       buf[ISERIES_LINE_LENGTH], protocol[9];
  iseries_t *iseries;

  /* Save trace format for passing between packets */
  iseries                = (iseries_t *) g_malloc (sizeof (iseries_t));
  wth->priv              = (void *) iseries;
  iseries->have_date     = FALSE;
  iseries->format        = format;

  for (line = 0; line < ISERIES_HDR_LINES_TO_CHECK; line++)
    {
      if (file_gets (buf, ISERIES_LINE_LENGTH, wth->fh) == NULL)
        {
          /* EOF or error. */
          *err = file_error (wth->fh, err_info);
          if (*err == WTAP_ERR_SHORT_READ)
            *err = 0;
          return FALSE;
        }

        /*
         * Check that we are dealing with an ETHERNET trace
         */
        if (iseries->format == ISERIES_FORMAT_UNICODE)
          {
            iseries_UNICODE_to_ASCII ((guint8 *)buf, ISERIES_LINE_LENGTH);
          }
        ascii_strup_inplace (buf);
        num_items_scanned = sscanf (buf,
                                   "%*[ \n\t]OBJECT PROTOCOL%*[ .:\n\t]%8s",
                                   protocol);
        if (num_items_scanned == 1)
          {
            if (memcmp (protocol, "ETHERNET", 8) != 0)
              return FALSE;
          }

        /*
         * The header is the only place where the date part of the timestamp is held, so
         * extract it here and store for all packets to access
         */
        num_items_scanned = sscanf (buf,
                                    "%*[ \n\t]START DATE/TIME%*[ .:\n\t]%2d/%2d/%2d",
                                    &iseries->month, &iseries->day,
                                    &iseries->year);
        if (num_items_scanned == 3)
          {
            iseries->have_date = TRUE;
          }
    }
  *err = 0;
  return TRUE;
}