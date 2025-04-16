TEE_Result TEE_MACComputeFinal(TEE_OperationHandle operation,
			       const void *message, uint32_t messageLen,
			       void *mac, uint32_t *macLen)
{
	TEE_Result res;
	uint64_t ml;

	if (operation == TEE_HANDLE_NULL ||
	    (message == NULL && messageLen != 0) ||
	    mac == NULL ||
	    macLen == NULL) {
		res = TEE_ERROR_BAD_PARAMETERS;
		goto out;
	}

	if (operation->info.operationClass != TEE_OPERATION_MAC) {
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

	ml = *macLen;
	res = utee_hash_final(operation->state, message, messageLen, mac, &ml);
	*macLen = ml;
	if (res != TEE_SUCCESS)
		goto out;

	operation->info.handleState &= ~TEE_HANDLE_FLAG_INITIALIZED;

	operation->operationState = TEE_OPERATION_STATE_INITIAL;

out:
	if (res != TEE_SUCCESS &&
	    res != TEE_ERROR_SHORT_BUFFER)
		TEE_Panic(res);

	return res;
}