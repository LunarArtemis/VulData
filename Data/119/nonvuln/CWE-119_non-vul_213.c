static void set_reg_mkey_seg(struct mlx5_mkey_seg *seg,
			     struct mlx5_ib_mr *mr,
			     u32 key, int access)
{
	int ndescs = ALIGN(mr->ndescs, 8) >> 1;

	memset(seg, 0, sizeof(*seg));

	if (mr->access_mode == MLX5_MKC_ACCESS_MODE_MTT)
		seg->log2_page_size = ilog2(mr->ibmr.page_size);
	else if (mr->access_mode == MLX5_MKC_ACCESS_MODE_KLMS)
		/* KLMs take twice the size of MTTs */
		ndescs *= 2;

	seg->flags = get_umr_flags(access) | mr->access_mode;
	seg->qpn_mkey7_0 = cpu_to_be32((key & 0xff) | 0xffffff00);
	seg->flags_pd = cpu_to_be32(MLX5_MKEY_REMOTE_INVAL);
	seg->start_addr = cpu_to_be64(mr->ibmr.iova);
	seg->len = cpu_to_be64(mr->ibmr.length);
	seg->xlt_oct_size = cpu_to_be32(ndescs);
}