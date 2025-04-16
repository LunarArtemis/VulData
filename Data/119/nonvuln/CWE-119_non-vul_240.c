static __be64 get_umr_disable_mr_mask(void)
{
	u64 result;

	result = MLX5_MKEY_MASK_FREE;

	return cpu_to_be64(result);
}