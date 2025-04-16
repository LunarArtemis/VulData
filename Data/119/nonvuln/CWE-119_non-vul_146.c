static void set_reg_mkey_segment(struct mlx5_mkey_seg *seg,
				 const struct ib_send_wr *wr)
{
	const struct mlx5_umr_wr *umrwr = umr_wr(wr);

	memset(seg, 0, sizeof(*seg));
	if (wr->send_flags & MLX5_IB_SEND_UMR_DISABLE_MR)
		seg->status = MLX5_MKEY_STATUS_FREE;

	seg->flags = convert_access(umrwr->access_flags);
	if (umrwr->pd)
		seg->flags_pd = cpu_to_be32(to_mpd(umrwr->pd)->pdn);
	if (wr->send_flags & MLX5_IB_SEND_UMR_UPDATE_TRANSLATION &&
	    !umrwr->length)
		seg->flags_pd |= cpu_to_be32(MLX5_MKEY_LEN64);

	seg->start_addr = cpu_to_be64(umrwr->virt_addr);
	seg->len = cpu_to_be64(umrwr->length);
	seg->log2_page_size = umrwr->page_shift;
	seg->qpn_mkey7_0 = cpu_to_be32(0xffffff00 |
				       mlx5_mkey_variant(umrwr->mkey));
}