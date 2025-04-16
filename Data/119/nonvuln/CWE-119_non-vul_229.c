int mlx5_ib_post_send(struct ib_qp *ibqp, const struct ib_send_wr *wr,
		      const struct ib_send_wr **bad_wr)
{
	return _mlx5_ib_post_send(ibqp, wr, bad_wr, false);
}