enum nss_status
_nss_dns_getnetbyname_r (const char *name, struct netent *result,
			 char *buffer, size_t buflen, int *errnop,
			 int *herrnop)
{
  /* Return entry for network with NAME.  */
  union
  {
    querybuf *buf;
    u_char *ptr;
  } net_buffer;
  querybuf *orig_net_buffer;
  int anslen;
  char *qbuf;
  enum nss_status status;

  if (__res_maybe_init (&_res, 0) == -1)
    return NSS_STATUS_UNAVAIL;

  qbuf = strdupa (name);

  net_buffer.buf = orig_net_buffer = (querybuf *) alloca (1024);

  anslen = __libc_res_nsearch (&_res, qbuf, C_IN, T_PTR, net_buffer.buf->buf,
			       1024, &net_buffer.ptr, NULL, NULL, NULL, NULL);
  if (anslen < 0)
    {
      /* Nothing found.  */
      *errnop = errno;
      if (net_buffer.buf != orig_net_buffer)
	free (net_buffer.buf);
      return (errno == ECONNREFUSED
	      || errno == EPFNOSUPPORT
	      || errno == EAFNOSUPPORT)
	? NSS_STATUS_UNAVAIL : NSS_STATUS_NOTFOUND;
    }

  status = getanswer_r (net_buffer.buf, anslen, result, buffer, buflen,
			errnop, herrnop, BYNAME);
  if (net_buffer.buf != orig_net_buffer)
    free (net_buffer.buf);
  return status;
}