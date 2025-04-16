static int set_rq_size(struct mlx5_ib_dev *dev, struct ib_qp_cap *cap,
		       int has_rq, struct mlx5_ib_qp *qp, struct mlx5_ib_create_qp *ucmd)
{
	int wqe_size;
	int wq_size;

	/* Sanity check RQ size before proceeding */
	if (cap->max_recv_wr > (1 << MLX5_CAP_GEN(dev->mdev, log_max_qp_sz)))
		return -EINVAL;

	if (!has_rq) {
		qp->rq.max_gs = 0;
		qp->rq.wqe_cnt = 0;
		qp->rq.wqe_shift = 0;
		cap->max_recv_wr = 0;
		cap->max_recv_sge = 0;
	} else {
		if (ucmd) {
			qp->rq.wqe_cnt = ucmd->rq_wqe_count;
			if (ucmd->rq_wqe_shift > BITS_PER_BYTE * sizeof(ucmd->rq_wqe_shift))
				return -EINVAL;
			qp->rq.wqe_shift = ucmd->rq_wqe_shift;
			if ((1 << qp->rq.wqe_shift) / sizeof(struct mlx5_wqe_data_seg) < qp->wq_sig)
				return -EINVAL;
			qp->rq.max_gs = (1 << qp->rq.wqe_shift) / sizeof(struct mlx5_wqe_data_seg) - qp->wq_sig;
			qp->rq.max_post = qp->rq.wqe_cnt;
		} else {
			wqe_size = qp->wq_sig ? sizeof(struct mlx5_wqe_signature_seg) : 0;
			wqe_size += cap->max_recv_sge * sizeof(struct mlx5_wqe_data_seg);
			wqe_size = roundup_pow_of_two(wqe_size);
			wq_size = roundup_pow_of_two(cap->max_recv_wr) * wqe_size;
			wq_size = max_t(int, wq_size, MLX5_SEND_WQE_BB);
			qp->rq.wqe_cnt = wq_size / wqe_size;
			if (wqe_size > MLX5_CAP_GEN(dev->mdev, max_wqe_sz_rq)) {
				mlx5_ib_dbg(dev, "wqe_size %d, max %d\n",
					    wqe_size,
					    MLX5_CAP_GEN(dev->mdev,
							 max_wqe_sz_rq));
				return -EINVAL;
			}
			qp->rq.wqe_shift = ilog2(wqe_size);
			qp->rq.max_gs = (1 << qp->rq.wqe_shift) / sizeof(struct mlx5_wqe_data_seg) - qp->wq_sig;
			qp->rq.max_post = qp->rq.wqe_cnt;
		}
	}

	return 0;
}