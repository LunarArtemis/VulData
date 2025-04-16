static u64 get_xlt_octo(u64 bytes)
{
	return ALIGN(bytes, MLX5_IB_UMR_XLT_ALIGNMENT) /
	       MLX5_IB_UMR_OCTOWORD;
}