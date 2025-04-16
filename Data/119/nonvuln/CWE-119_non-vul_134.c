static int set_reg_wr(struct mlx5_ib_qp *qp,
		      const struct ib_reg_wr *wr,
		      void **seg, int *size)
{
	struct mlx5_ib_mr *mr = to_mmr(wr->mr);
	struct mlx5_ib_pd *pd = to_mpd(qp->ibqp.pd);
	int mr_list_size = mr->ndescs * mr->desc_size;
	bool umr_inline = mr_list_size <= MLX5_IB_SQ_UMR_INLINE_THRESHOLD;

	if (unlikely(wr->wr.send_flags & IB_SEND_INLINE)) {
		mlx5_ib_warn(to_mdev(qp->ibqp.device),
			     "Invalid IB_SEND_INLINE send flag\n");
		return -EINVAL;
	}

	set_reg_umr_seg(*seg, mr, umr_inline);
	*seg += sizeof(struct mlx5_wqe_umr_ctrl_seg);
	*size += sizeof(struct mlx5_wqe_umr_ctrl_seg) / 16;
	if (unlikely((*seg == qp->sq.qend)))
		*seg = mlx5_get_send_wqe(qp, 0);

	set_reg_mkey_seg(*seg, mr, wr->key, wr->access);
	*seg += sizeof(struct mlx5_mkey_seg);
	*size += sizeof(struct mlx5_mkey_seg) / 16;
	if (unlikely((*seg == qp->sq.qend)))
		*seg = mlx5_get_send_wqe(qp, 0);

	if (umr_inline) {
		set_reg_umr_inline_seg(*seg, qp, mr, mr_list_size);
		*size += get_xlt_octo(mr_list_size);
	} else {
		set_reg_data_seg(*seg, mr, pd);
		*seg += sizeof(struct mlx5_wqe_data_seg);
		*size += (sizeof(struct mlx5_wqe_data_seg) / 16);
	}
	return 0;
}