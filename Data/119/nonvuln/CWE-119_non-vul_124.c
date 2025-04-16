static void set_reg_umr_seg(struct mlx5_wqe_umr_ctrl_seg *umr,
			    struct mlx5_ib_mr *mr, bool umr_inline)
{
	int size = mr->ndescs * mr->desc_size;

	memset(umr, 0, sizeof(*umr));

	umr->flags = MLX5_UMR_CHECK_NOT_FREE;
	if (umr_inline)
		umr->flags |= MLX5_UMR_INLINE;
	umr->xlt_octowords = cpu_to_be16(get_xlt_octo(size));
	umr->mkey_mask = frwr_mkey_mask();
}