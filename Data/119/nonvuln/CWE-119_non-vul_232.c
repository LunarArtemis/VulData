static int create_raw_packet_qp_rq(struct mlx5_ib_dev *dev,
				   struct mlx5_ib_rq *rq, void *qpin,
				   size_t qpinlen)
{
	struct mlx5_ib_qp *mqp = rq->base.container_mibqp;
	__be64 *pas;
	__be64 *qp_pas;
	void *in;
	void *rqc;
	void *wq;
	void *qpc = MLX5_ADDR_OF(create_qp_in, qpin, qpc);
	size_t rq_pas_size = get_rq_pas_size(qpc);
	size_t inlen;
	int err;

	if (qpinlen < rq_pas_size + MLX5_BYTE_OFF(create_qp_in, pas))
		return -EINVAL;

	inlen = MLX5_ST_SZ_BYTES(create_rq_in) + rq_pas_size;
	in = kvzalloc(inlen, GFP_KERNEL);
	if (!in)
		return -ENOMEM;

	rqc = MLX5_ADDR_OF(create_rq_in, in, ctx);
	if (!(rq->flags & MLX5_IB_RQ_CVLAN_STRIPPING))
		MLX5_SET(rqc, rqc, vsd, 1);
	MLX5_SET(rqc, rqc, mem_rq_type, MLX5_RQC_MEM_RQ_TYPE_MEMORY_RQ_INLINE);
	MLX5_SET(rqc, rqc, state, MLX5_RQC_STATE_RST);
	MLX5_SET(rqc, rqc, flush_in_error_en, 1);
	MLX5_SET(rqc, rqc, user_index, MLX5_GET(qpc, qpc, user_index));
	MLX5_SET(rqc, rqc, cqn, MLX5_GET(qpc, qpc, cqn_rcv));

	if (mqp->flags & MLX5_IB_QP_CAP_SCATTER_FCS)
		MLX5_SET(rqc, rqc, scatter_fcs, 1);

	wq = MLX5_ADDR_OF(rqc, rqc, wq);
	MLX5_SET(wq, wq, wq_type, MLX5_WQ_TYPE_CYCLIC);
	if (rq->flags & MLX5_IB_RQ_PCI_WRITE_END_PADDING)
		MLX5_SET(wq, wq, end_padding_mode, MLX5_WQ_END_PAD_MODE_ALIGN);
	MLX5_SET(wq, wq, page_offset, MLX5_GET(qpc, qpc, page_offset));
	MLX5_SET(wq, wq, pd, MLX5_GET(qpc, qpc, pd));
	MLX5_SET64(wq, wq, dbr_addr, MLX5_GET64(qpc, qpc, dbr_addr));
	MLX5_SET(wq, wq, log_wq_stride, MLX5_GET(qpc, qpc, log_rq_stride) + 4);
	MLX5_SET(wq, wq, log_wq_pg_sz, MLX5_GET(qpc, qpc, log_page_size));
	MLX5_SET(wq, wq, log_wq_sz, MLX5_GET(qpc, qpc, log_rq_size));

	pas = (__be64 *)MLX5_ADDR_OF(wq, wq, pas);
	qp_pas = (__be64 *)MLX5_ADDR_OF(create_qp_in, qpin, pas);
	memcpy(pas, qp_pas, rq_pas_size);

	err = mlx5_core_create_rq_tracked(dev->mdev, in, inlen, &rq->base.mqp);

	kvfree(in);

	return err;
}