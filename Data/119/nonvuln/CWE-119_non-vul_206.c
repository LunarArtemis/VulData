static int calc_sq_size(struct mlx5_ib_dev *dev, struct ib_qp_init_attr *attr,
			struct mlx5_ib_qp *qp)
{
	int wqe_size;
	int wq_size;

	if (!attr->cap.max_send_wr)
		return 0;

	wqe_size = calc_send_wqe(attr);
	mlx5_ib_dbg(dev, "wqe_size %d\n", wqe_size);
	if (wqe_size < 0)
		return wqe_size;

	if (wqe_size > MLX5_CAP_GEN(dev->mdev, max_wqe_sz_sq)) {
		mlx5_ib_dbg(dev, "wqe_size(%d) > max_sq_desc_sz(%d)\n",
			    wqe_size, MLX5_CAP_GEN(dev->mdev, max_wqe_sz_sq));
		return -EINVAL;
	}

	qp->max_inline_data = wqe_size - sq_overhead(attr) -
			      sizeof(struct mlx5_wqe_inline_seg);
	attr->cap.max_inline_data = qp->max_inline_data;

	if (attr->create_flags & IB_QP_CREATE_SIGNATURE_EN)
		qp->signature_en = true;

	wq_size = roundup_pow_of_two(attr->cap.max_send_wr * wqe_size);
	qp->sq.wqe_cnt = wq_size / MLX5_SEND_WQE_BB;
	if (qp->sq.wqe_cnt > (1 << MLX5_CAP_GEN(dev->mdev, log_max_qp_sz))) {
		mlx5_ib_dbg(dev, "send queue size (%d * %d / %d -> %d) exceeds limits(%d)\n",
			    attr->cap.max_send_wr, wqe_size, MLX5_SEND_WQE_BB,
			    qp->sq.wqe_cnt,
			    1 << MLX5_CAP_GEN(dev->mdev, log_max_qp_sz));
		return -ENOMEM;
	}
	qp->sq.wqe_shift = ilog2(MLX5_SEND_WQE_BB);
	qp->sq.max_gs = get_send_sge(attr, wqe_size);
	if (qp->sq.max_gs < attr->cap.max_send_sge)
		return -ENOMEM;

	attr->cap.max_send_sge = qp->sq.max_gs;
	qp->sq.max_post = wq_size / wqe_size;
	attr->cap.max_send_wr = qp->sq.max_post;

	return wq_size;
}