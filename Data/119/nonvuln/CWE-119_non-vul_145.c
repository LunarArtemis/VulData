static void set_linv_umr_seg(struct mlx5_wqe_umr_ctrl_seg *umr)
{
	memset(umr, 0, sizeof(*umr));
	umr->mkey_mask = cpu_to_be64(MLX5_MKEY_MASK_FREE);
	umr->flags = MLX5_UMR_INLINE;
}