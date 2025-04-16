static __be64 get_umr_update_access_mask(int atomic)
{
	u64 result;

	result = MLX5_MKEY_MASK_LR |
		 MLX5_MKEY_MASK_LW |
		 MLX5_MKEY_MASK_RR |
		 MLX5_MKEY_MASK_RW;

	if (atomic)
		result |= MLX5_MKEY_MASK_A;

	return cpu_to_be64(result);
}