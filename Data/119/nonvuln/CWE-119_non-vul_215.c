void mlx5_ib_drain_sq(struct ib_qp *qp)
{
	struct ib_cq *cq = qp->send_cq;
	struct ib_qp_attr attr = { .qp_state = IB_QPS_ERR };
	struct mlx5_ib_drain_cqe sdrain;
	const struct ib_send_wr *bad_swr;
	struct ib_rdma_wr swr = {
		.wr = {
			.next = NULL,
			{ .wr_cqe	= &sdrain.cqe, },
			.opcode	= IB_WR_RDMA_WRITE,
		},
	};
	int ret;
	struct mlx5_ib_dev *dev = to_mdev(qp->device);
	struct mlx5_core_dev *mdev = dev->mdev;

	ret = ib_modify_qp(qp, &attr, IB_QP_STATE);
	if (ret && mdev->state != MLX5_DEVICE_STATE_INTERNAL_ERROR) {
		WARN_ONCE(ret, "failed to drain send queue: %d\n", ret);
		return;
	}

	sdrain.cqe.done = mlx5_ib_drain_qp_done;
	init_completion(&sdrain.done);

	ret = _mlx5_ib_post_send(qp, &swr.wr, &bad_swr, true);
	if (ret) {
		WARN_ONCE(ret, "failed to drain send queue: %d\n", ret);
		return;
	}

	handle_drain_completion(cq, &sdrain, dev);
}