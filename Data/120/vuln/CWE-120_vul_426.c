static int get_cert_name(gnutls_x509_crt_t cert, char *name, size_t namelen)
{
	if (gnutls_x509_crt_get_dn_by_oid(cert, GNUTLS_OID_X520_COMMON_NAME,
					  0, 0, name, &namelen) &&
	    gnutls_x509_crt_get_dn(cert, name, &namelen)) {
		name[namelen-1] = 0;
		snprintf(name, namelen-1, "<unknown>");
		return -EINVAL;
	}
	return 0;
}