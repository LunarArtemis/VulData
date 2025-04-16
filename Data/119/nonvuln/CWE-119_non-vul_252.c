int mlx5_ib_post_recv(struct ib_qp *ibqp, const struct ib_recv_wr *wr,
		      const struct ib_recv_wr **bad_wr)
{
	return _mlx5_ib_post_recv(ibqp, wr, bad_wr, false);
}