void TEE_MACUpdate(TEE_OperationHandle operation, const void *chunk,
		   uint32_t chunkSize)
{
	TEE_Result res;

	if (operation == TEE_HANDLE_NULL || (chunk == NULL && chunkSize != 0))
		TEE_Panic(0);

	if (operation->info.operationClass != TEE_OPERATION_MAC)
		TEE_Panic(0);

	if ((operation->info.handleState & TEE_HANDLE_FLAG_INITIALIZED) == 0)
		TEE_Panic(0);

	if (operation->operationState != TEE_OPERATION_STATE_ACTIVE)
		TEE_Panic(0);

	res = utee_hash_update(operation->state, chunk, chunkSize);
	if (res != TEE_SUCCESS)
		TEE_Panic(res);
}