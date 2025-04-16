static void mlx5_ib_drain_qp_done(struct ib_cq *cq, struct ib_wc *wc)
{
	struct mlx5_ib_drain_cqe *cqe = container_of(wc->wr_cqe,
						     struct mlx5_ib_drain_cqe,
						     cqe);

	complete(&cqe->done);
}