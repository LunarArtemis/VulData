static int set_user_buf_size(struct mlx5_ib_dev *dev,
			    struct mlx5_ib_qp *qp,
			    struct mlx5_ib_create_qp *ucmd,
			    struct mlx5_ib_qp_base *base,
			    struct ib_qp_init_attr *attr)
{
	int desc_sz = 1 << qp->sq.wqe_shift;

	if (desc_sz > MLX5_CAP_GEN(dev->mdev, max_wqe_sz_sq)) {
		mlx5_ib_warn(dev, "desc_sz %d, max_sq_desc_sz %d\n",
			     desc_sz, MLX5_CAP_GEN(dev->mdev, max_wqe_sz_sq));
		return -EINVAL;
	}

	if (ucmd->sq_wqe_count && ((1 << ilog2(ucmd->sq_wqe_count)) != ucmd->sq_wqe_count)) {
		mlx5_ib_warn(dev, "sq_wqe_count %d, sq_wqe_count %d\n",
			     ucmd->sq_wqe_count, ucmd->sq_wqe_count);
		return -EINVAL;
	}

	qp->sq.wqe_cnt = ucmd->sq_wqe_count;

	if (qp->sq.wqe_cnt > (1 << MLX5_CAP_GEN(dev->mdev, log_max_qp_sz))) {
		mlx5_ib_warn(dev, "wqe_cnt %d, max_wqes %d\n",
			     qp->sq.wqe_cnt,
			     1 << MLX5_CAP_GEN(dev->mdev, log_max_qp_sz));
		return -EINVAL;
	}

	if (attr->qp_type == IB_QPT_RAW_PACKET ||
	    qp->flags & MLX5_IB_QP_UNDERLAY) {
		base->ubuffer.buf_size = qp->rq.wqe_cnt << qp->rq.wqe_shift;
		qp->raw_packet_qp.sq.ubuffer.buf_size = qp->sq.wqe_cnt << 6;
	} else {
		base->ubuffer.buf_size = (qp->rq.wqe_cnt << qp->rq.wqe_shift) +
					 (qp->sq.wqe_cnt << 6);
	}

	return 0;
}