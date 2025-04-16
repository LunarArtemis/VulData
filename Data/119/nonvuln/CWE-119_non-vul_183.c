static int set_sig_umr_wr(const struct ib_send_wr *send_wr,
			  struct mlx5_ib_qp *qp, void **seg, int *size)
{
	const struct ib_sig_handover_wr *wr = sig_handover_wr(send_wr);
	struct mlx5_ib_mr *sig_mr = to_mmr(wr->sig_mr);
	u32 pdn = get_pd(qp)->pdn;
	u32 xlt_size;
	int region_len, ret;

	if (unlikely(wr->wr.num_sge != 1) ||
	    unlikely(wr->access_flags & IB_ACCESS_REMOTE_ATOMIC) ||
	    unlikely(!sig_mr->sig) || unlikely(!qp->signature_en) ||
	    unlikely(!sig_mr->sig->sig_status_checked))
		return -EINVAL;

	/* length of the protected region, data + protection */
	region_len = wr->wr.sg_list->length;
	if (wr->prot &&
	    (wr->prot->lkey != wr->wr.sg_list->lkey  ||
	     wr->prot->addr != wr->wr.sg_list->addr  ||
	     wr->prot->length != wr->wr.sg_list->length))
		region_len += wr->prot->length;

	/**
	 * KLM octoword size - if protection was provided
	 * then we use strided block format (3 octowords),
	 * else we use single KLM (1 octoword)
	 **/
	xlt_size = wr->prot ? 0x30 : sizeof(struct mlx5_klm);

	set_sig_umr_segment(*seg, xlt_size);
	*seg += sizeof(struct mlx5_wqe_umr_ctrl_seg);
	*size += sizeof(struct mlx5_wqe_umr_ctrl_seg) / 16;
	if (unlikely((*seg == qp->sq.qend)))
		*seg = mlx5_get_send_wqe(qp, 0);

	set_sig_mkey_segment(*seg, wr, xlt_size, region_len, pdn);
	*seg += sizeof(struct mlx5_mkey_seg);
	*size += sizeof(struct mlx5_mkey_seg) / 16;
	if (unlikely((*seg == qp->sq.qend)))
		*seg = mlx5_get_send_wqe(qp, 0);

	ret = set_sig_data_segment(wr, qp, seg, size);
	if (ret)
		return ret;

	sig_mr->sig->sig_status_checked = false;
	return 0;
}