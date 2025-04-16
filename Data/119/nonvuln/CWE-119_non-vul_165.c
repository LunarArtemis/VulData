static void destroy_raw_packet_qp(struct mlx5_ib_dev *dev,
				  struct mlx5_ib_qp *qp)
{
	struct mlx5_ib_raw_packet_qp *raw_packet_qp = &qp->raw_packet_qp;
	struct mlx5_ib_sq *sq = &raw_packet_qp->sq;
	struct mlx5_ib_rq *rq = &raw_packet_qp->rq;

	if (qp->rq.wqe_cnt) {
		destroy_raw_packet_qp_tir(dev, rq);
		destroy_raw_packet_qp_rq(dev, rq);
	}

	if (qp->sq.wqe_cnt) {
		destroy_raw_packet_qp_sq(dev, sq);
		destroy_raw_packet_qp_tis(dev, sq);
	}
}