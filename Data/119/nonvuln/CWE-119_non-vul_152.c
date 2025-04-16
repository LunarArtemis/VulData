static void destroy_qp_common(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp)
{
	struct mlx5_ib_cq *send_cq, *recv_cq;
	struct mlx5_ib_qp_base *base;
	unsigned long flags;
	int err;

	if (qp->ibqp.rwq_ind_tbl) {
		destroy_rss_raw_qp_tir(dev, qp);
		return;
	}

	base = (qp->ibqp.qp_type == IB_QPT_RAW_PACKET ||
		qp->flags & MLX5_IB_QP_UNDERLAY) ?
	       &qp->raw_packet_qp.rq.base :
	       &qp->trans_qp.base;

	if (qp->state != IB_QPS_RESET) {
		if (qp->ibqp.qp_type != IB_QPT_RAW_PACKET &&
		    !(qp->flags & MLX5_IB_QP_UNDERLAY)) {
			err = mlx5_core_qp_modify(dev->mdev,
						  MLX5_CMD_OP_2RST_QP, 0,
						  NULL, &base->mqp);
		} else {
			struct mlx5_modify_raw_qp_param raw_qp_param = {
				.operation = MLX5_CMD_OP_2RST_QP
			};

			err = modify_raw_packet_qp(dev, qp, &raw_qp_param, 0);
		}
		if (err)
			mlx5_ib_warn(dev, "mlx5_ib: modify QP 0x%06x to RESET failed\n",
				     base->mqp.qpn);
	}

	get_cqs(qp->ibqp.qp_type, qp->ibqp.send_cq, qp->ibqp.recv_cq,
		&send_cq, &recv_cq);

	spin_lock_irqsave(&dev->reset_flow_resource_lock, flags);
	mlx5_ib_lock_cqs(send_cq, recv_cq);
	/* del from lists under both locks above to protect reset flow paths */
	list_del(&qp->qps_list);
	if (send_cq)
		list_del(&qp->cq_send_list);

	if (recv_cq)
		list_del(&qp->cq_recv_list);

	if (qp->create_type == MLX5_QP_KERNEL) {
		__mlx5_ib_cq_clean(recv_cq, base->mqp.qpn,
				   qp->ibqp.srq ? to_msrq(qp->ibqp.srq) : NULL);
		if (send_cq != recv_cq)
			__mlx5_ib_cq_clean(send_cq, base->mqp.qpn,
					   NULL);
	}
	mlx5_ib_unlock_cqs(send_cq, recv_cq);
	spin_unlock_irqrestore(&dev->reset_flow_resource_lock, flags);

	if (qp->ibqp.qp_type == IB_QPT_RAW_PACKET ||
	    qp->flags & MLX5_IB_QP_UNDERLAY) {
		destroy_raw_packet_qp(dev, qp);
	} else {
		err = mlx5_core_destroy_qp(dev->mdev, &base->mqp);
		if (err)
			mlx5_ib_warn(dev, "failed to destroy QP 0x%x\n",
				     base->mqp.qpn);
	}

	if (qp->create_type == MLX5_QP_KERNEL)
		destroy_qp_kernel(dev, qp);
	else if (qp->create_type == MLX5_QP_USER)
		destroy_qp_user(dev, &get_pd(qp)->ibpd, qp, base);
}