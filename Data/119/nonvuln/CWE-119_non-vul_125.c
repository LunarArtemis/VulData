static struct mlx5_ib_pd *get_pd(struct mlx5_ib_qp *qp)
{
	return to_mpd(qp->ibqp.pd);
}