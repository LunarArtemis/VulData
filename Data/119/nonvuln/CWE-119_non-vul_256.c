int mlx5_ib_query_qp(struct ib_qp *ibqp, struct ib_qp_attr *qp_attr,
		     int qp_attr_mask, struct ib_qp_init_attr *qp_init_attr)
{
	struct mlx5_ib_dev *dev = to_mdev(ibqp->device);
	struct mlx5_ib_qp *qp = to_mqp(ibqp);
	int err = 0;
	u8 raw_packet_qp_state;

	if (ibqp->rwq_ind_tbl)
		return -ENOSYS;

	if (unlikely(ibqp->qp_type == IB_QPT_GSI))
		return mlx5_ib_gsi_query_qp(ibqp, qp_attr, qp_attr_mask,
					    qp_init_attr);

	/* Not all of output fields are applicable, make sure to zero them */
	memset(qp_init_attr, 0, sizeof(*qp_init_attr));
	memset(qp_attr, 0, sizeof(*qp_attr));

	if (unlikely(qp->qp_sub_type == MLX5_IB_QPT_DCT))
		return mlx5_ib_dct_query_qp(dev, qp, qp_attr,
					    qp_attr_mask, qp_init_attr);

	mutex_lock(&qp->mutex);

	if (qp->ibqp.qp_type == IB_QPT_RAW_PACKET ||
	    qp->flags & MLX5_IB_QP_UNDERLAY) {
		err = query_raw_packet_qp_state(dev, qp, &raw_packet_qp_state);
		if (err)
			goto out;
		qp->state = raw_packet_qp_state;
		qp_attr->port_num = 1;
	} else {
		err = query_qp_attr(dev, qp, qp_attr);
		if (err)
			goto out;
	}

	qp_attr->qp_state	     = qp->state;
	qp_attr->cur_qp_state	     = qp_attr->qp_state;
	qp_attr->cap.max_recv_wr     = qp->rq.wqe_cnt;
	qp_attr->cap.max_recv_sge    = qp->rq.max_gs;

	if (!ibqp->uobject) {
		qp_attr->cap.max_send_wr  = qp->sq.max_post;
		qp_attr->cap.max_send_sge = qp->sq.max_gs;
		qp_init_attr->qp_context = ibqp->qp_context;
	} else {
		qp_attr->cap.max_send_wr  = 0;
		qp_attr->cap.max_send_sge = 0;
	}

	qp_init_attr->qp_type = ibqp->qp_type;
	qp_init_attr->recv_cq = ibqp->recv_cq;
	qp_init_attr->send_cq = ibqp->send_cq;
	qp_init_attr->srq = ibqp->srq;
	qp_attr->cap.max_inline_data = qp->max_inline_data;

	qp_init_attr->cap	     = qp_attr->cap;

	qp_init_attr->create_flags = 0;
	if (qp->flags & MLX5_IB_QP_BLOCK_MULTICAST_LOOPBACK)
		qp_init_attr->create_flags |= IB_QP_CREATE_BLOCK_MULTICAST_LOOPBACK;

	if (qp->flags & MLX5_IB_QP_CROSS_CHANNEL)
		qp_init_attr->create_flags |= IB_QP_CREATE_CROSS_CHANNEL;
	if (qp->flags & MLX5_IB_QP_MANAGED_SEND)
		qp_init_attr->create_flags |= IB_QP_CREATE_MANAGED_SEND;
	if (qp->flags & MLX5_IB_QP_MANAGED_RECV)
		qp_init_attr->create_flags |= IB_QP_CREATE_MANAGED_RECV;
	if (qp->flags & MLX5_IB_QP_SQPN_QP1)
		qp_init_attr->create_flags |= mlx5_ib_create_qp_sqpn_qp1();

	qp_init_attr->sq_sig_type = qp->sq_signal_bits & MLX5_WQE_CTRL_CQ_UPDATE ?
		IB_SIGNAL_ALL_WR : IB_SIGNAL_REQ_WR;

out:
	mutex_unlock(&qp->mutex);
	return err;
}