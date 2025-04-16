int phar_parse_metadata(char **buffer, zval **metadata, int zip_metadata_len TSRMLS_DC) /* {{{ */
{
	const unsigned char *p;
	php_uint32 buf_len;
	php_unserialize_data_t var_hash;

	if (!zip_metadata_len) {
		PHAR_GET_32(*buffer, buf_len);
	} else {
		buf_len = zip_metadata_len;
	}

	if (buf_len) {
		ALLOC_ZVAL(*metadata);
		INIT_ZVAL(**metadata);
		p = (const unsigned char*) *buffer;
		PHP_VAR_UNSERIALIZE_INIT(var_hash);

		if (!php_var_unserialize(metadata, &p, p + buf_len, &var_hash TSRMLS_CC)) {
			PHP_VAR_UNSERIALIZE_DESTROY(var_hash);
			zval_ptr_dtor(metadata);
			*metadata = NULL;
			return FAILURE;
		}

		PHP_VAR_UNSERIALIZE_DESTROY(var_hash);

		if (PHAR_G(persist)) {
			/* lazy init metadata */
			zval_ptr_dtor(metadata);
			*metadata = (zval *) pemalloc(buf_len, 1);
			memcpy(*metadata, *buffer, buf_len);
			*buffer += buf_len;
			return SUCCESS;
		}
	} else {
		*metadata = NULL;
	}

	if (!zip_metadata_len) {
		*buffer += buf_len;
	}

	return SUCCESS;
}