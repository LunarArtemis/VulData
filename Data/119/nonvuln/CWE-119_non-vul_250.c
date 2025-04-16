static __be64 get_umr_update_translation_mask(void)
{
	u64 result;

	result = MLX5_MKEY_MASK_LEN |
		 MLX5_MKEY_MASK_PAGE_SIZE |
		 MLX5_MKEY_MASK_START_ADDR;

	return cpu_to_be64(result);
}