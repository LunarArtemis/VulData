TEE_Result TEE_CipherUpdate(TEE_OperationHandle operation, const void *srcData,
			    uint32_t srcLen, void *destData, uint32_t *destLen)
{
	TEE_Result res;
	size_t req_dlen;
	uint64_t dl;

	if (operation == TEE_HANDLE_NULL ||
	    (srcData == NULL && srcLen != 0) ||
	    destLen == NULL ||
	    (destData == NULL && *destLen != 0)) {
		res = TEE_ERROR_BAD_PARAMETERS;
		goto out;
	}

	if (operation->info.operationClass != TEE_OPERATION_CIPHER) {
		res = TEE_ERROR_BAD_PARAMETERS;
		goto out;
	}

	if ((operation->info.handleState & TEE_HANDLE_FLAG_INITIALIZED) == 0) {
		res = TEE_ERROR_BAD_PARAMETERS;
		goto out;
	}

	if (operation->operationState != TEE_OPERATION_STATE_ACTIVE) {
		res = TEE_ERROR_BAD_PARAMETERS;
		goto out;
	}

	if (!srcData && !srcLen) {
		*destLen = 0;
		res = TEE_SUCCESS;
		goto out;
	}

	/* Calculate required dlen */
	if (operation->block_size > 1) {
		req_dlen = ((operation->buffer_offs + srcLen) /
			    operation->block_size) * operation->block_size;
	} else {
		req_dlen = srcLen;
	}
	if (operation->buffer_two_blocks) {
		if (req_dlen > operation->block_size * 2)
			req_dlen -= operation->block_size * 2;
		else
			req_dlen = 0;
	}
	/*
	 * Check that required destLen is big enough before starting to feed
	 * data to the algorithm. Errors during feeding of data are fatal as we
	 * can't restore sync with this API.
	 */
	if (*destLen < req_dlen) {
		*destLen = req_dlen;
		res = TEE_ERROR_SHORT_BUFFER;
		goto out;
	}

	dl = *destLen;
	if (operation->block_size > 1) {
		res = tee_buffer_update(operation, utee_cipher_update, srcData,
					srcLen, destData, &dl);
	} else {
		if (srcLen > 0) {
			res = utee_cipher_update(operation->state, srcData,
						 srcLen, destData, &dl);
		} else {
			res = TEE_SUCCESS;
			dl = 0;
		}
	}
	*destLen = dl;

out:
	if (res != TEE_SUCCESS &&
	    res != TEE_ERROR_SHORT_BUFFER)
		TEE_Panic(res);

	return res;
}