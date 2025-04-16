static int create_raw_packet_qp(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp,
				u32 *in, size_t inlen,
				struct ib_pd *pd)
{
	struct mlx5_ib_raw_packet_qp *raw_packet_qp = &qp->raw_packet_qp;
	struct mlx5_ib_sq *sq = &raw_packet_qp->sq;
	struct mlx5_ib_rq *rq = &raw_packet_qp->rq;
	struct ib_uobject *uobj = pd->uobject;
	struct ib_ucontext *ucontext = uobj->context;
	struct mlx5_ib_ucontext *mucontext = to_mucontext(ucontext);
	int err;
	u32 tdn = mucontext->tdn;

	if (qp->sq.wqe_cnt) {
		err = create_raw_packet_qp_tis(dev, qp, sq, tdn);
		if (err)
			return err;

		err = create_raw_packet_qp_sq(dev, sq, in, pd);
		if (err)
			goto err_destroy_tis;

		sq->base.container_mibqp = qp;
		sq->base.mqp.event = mlx5_ib_qp_event;
	}

	if (qp->rq.wqe_cnt) {
		rq->base.container_mibqp = qp;

		if (qp->flags & MLX5_IB_QP_CVLAN_STRIPPING)
			rq->flags |= MLX5_IB_RQ_CVLAN_STRIPPING;
		if (qp->flags & MLX5_IB_QP_PCI_WRITE_END_PADDING)
			rq->flags |= MLX5_IB_RQ_PCI_WRITE_END_PADDING;
		err = create_raw_packet_qp_rq(dev, rq, in, inlen);
		if (err)
			goto err_destroy_sq;


		err = create_raw_packet_qp_tir(dev, rq, tdn,
					       qp->tunnel_offload_en);
		if (err)
			goto err_destroy_rq;
	}

	qp->trans_qp.base.mqp.qpn = qp->sq.wqe_cnt ? sq->base.mqp.qpn :
						     rq->base.mqp.qpn;

	return 0;

err_destroy_rq:
	destroy_raw_packet_qp_rq(dev, rq);
err_destroy_sq:
	if (!qp->sq.wqe_cnt)
		return err;
	destroy_raw_packet_qp_sq(dev, sq);
err_destroy_tis:
	destroy_raw_packet_qp_tis(dev, sq);

	return err;
}