void mlx5_ib_drain_rq(struct ib_qp *qp)
{
	struct ib_cq *cq = qp->recv_cq;
	struct ib_qp_attr attr = { .qp_state = IB_QPS_ERR };
	struct mlx5_ib_drain_cqe rdrain;
	struct ib_recv_wr rwr = {};
	const struct ib_recv_wr *bad_rwr;
	int ret;
	struct mlx5_ib_dev *dev = to_mdev(qp->device);
	struct mlx5_core_dev *mdev = dev->mdev;

	ret = ib_modify_qp(qp, &attr, IB_QP_STATE);
	if (ret && mdev->state != MLX5_DEVICE_STATE_INTERNAL_ERROR) {
		WARN_ONCE(ret, "failed to drain recv queue: %d\n", ret);
		return;
	}

	rwr.wr_cqe = &rdrain.cqe;
	rdrain.cqe.done = mlx5_ib_drain_qp_done;
	init_completion(&rdrain.done);

	ret = _mlx5_ib_post_recv(qp, &rwr, &bad_rwr, true);
	if (ret) {
		WARN_ONCE(ret, "failed to drain recv queue: %d\n", ret);
		return;
	}

	handle_drain_completion(cq, &rdrain, dev);
}