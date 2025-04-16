static int query_raw_packet_qp_rq_state(struct mlx5_ib_dev *dev,
					struct mlx5_ib_rq *rq,
					u8 *rq_state)
{
	void *out;
	void *rqc;
	int inlen;
	int err;

	inlen = MLX5_ST_SZ_BYTES(query_rq_out);
	out = kvzalloc(inlen, GFP_KERNEL);
	if (!out)
		return -ENOMEM;

	err = mlx5_core_query_rq(dev->mdev, rq->base.mqp.qpn, out);
	if (err)
		goto out;

	rqc = MLX5_ADDR_OF(query_rq_out, out, rq_context);
	*rq_state = MLX5_GET(rqc, rqc, state);
	rq->state = *rq_state;

out:
	kvfree(out);
	return err;
}