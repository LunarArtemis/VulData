static void set_linv_mkey_seg(struct mlx5_mkey_seg *seg)
{
	memset(seg, 0, sizeof(*seg));
	seg->status = MLX5_MKEY_STATUS_FREE;
}