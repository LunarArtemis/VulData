int mlx5_ib_read_user_wqe(struct mlx5_ib_qp *qp, int send, int wqe_index,
			  void *buffer, u32 length,
			  struct mlx5_ib_qp_base *base)
{
	struct ib_device *ibdev = qp->ibqp.device;
	struct mlx5_ib_dev *dev = to_mdev(ibdev);
	struct mlx5_ib_wq *wq = send ? &qp->sq : &qp->rq;
	size_t offset;
	size_t wq_end;
	struct ib_umem *umem = base->ubuffer.umem;
	u32 first_copy_length;
	int wqe_length;
	int ret;

	if (wq->wqe_cnt == 0) {
		mlx5_ib_dbg(dev, "mlx5_ib_read_user_wqe for a QP with wqe_cnt == 0. qp_type: 0x%x\n",
			    qp->ibqp.qp_type);
		return -EINVAL;
	}

	offset = wq->offset + ((wqe_index % wq->wqe_cnt) << wq->wqe_shift);
	wq_end = wq->offset + (wq->wqe_cnt << wq->wqe_shift);

	if (send && length < sizeof(struct mlx5_wqe_ctrl_seg))
		return -EINVAL;

	if (offset > umem->length ||
	    (send && offset + sizeof(struct mlx5_wqe_ctrl_seg) > umem->length))
		return -EINVAL;

	first_copy_length = min_t(u32, offset + length, wq_end) - offset;
	ret = ib_umem_copy_from(buffer, umem, offset, first_copy_length);
	if (ret)
		return ret;

	if (send) {
		struct mlx5_wqe_ctrl_seg *ctrl = buffer;
		int ds = be32_to_cpu(ctrl->qpn_ds) & MLX5_WQE_CTRL_DS_MASK;

		wqe_length = ds * MLX5_WQE_DS_UNITS;
	} else {
		wqe_length = 1 << wq->wqe_shift;
	}

	if (wqe_length <= first_copy_length)
		return first_copy_length;

	ret = ib_umem_copy_from(buffer + first_copy_length, umem, wq->offset,
				wqe_length - first_copy_length);
	if (ret)
		return ret;

	return wqe_length;
}