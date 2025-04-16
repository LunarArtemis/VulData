static gboolean
canonify_unencrypted_header(guchar *buff, guint32 *offset, guint32 buffsize)
{
  const TOP_ELEMENT_CONTROL *t = canonifyTable;
  guint32 len;

  for (t = canonifyTable; t->element != NULL; t++)
  {
    len = *(t->length);
    if (t->required && *(t->element) == NULL)
      return FALSE;
    if (*(t->element) != NULL) {
      if (t->addtag) {
        /* recreate original tag and length */
        buff[(*offset)++] = t->tag;
        (*offset) += encode_ber_len(&buff[*offset], len, 4);
      }
      if (t->truncate) {
        len = 3+2*get_ber_len_size(len);
      }
      /* bail out if the cannonization buffer is too small */
      /* this should never happen! */
      if (buffsize < *offset + len) {
        return FALSE;
      }
      memcpy(&buff[*offset], *(t->element), len);
      (*offset) += len;
      if (t->addtag) {
          *(t->element) = NULL;
      }
    }
  }
  return TRUE;
}