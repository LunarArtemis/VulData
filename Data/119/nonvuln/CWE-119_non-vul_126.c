static void raw_packet_qp_copy_info(struct mlx5_ib_qp *qp,
				    struct mlx5_ib_raw_packet_qp *raw_packet_qp)
{
	struct mlx5_ib_sq *sq = &raw_packet_qp->sq;
	struct mlx5_ib_rq *rq = &raw_packet_qp->rq;

	sq->sq = &qp->sq;
	rq->rq = &qp->rq;
	sq->doorbell = &qp->db;
	rq->doorbell = &qp->db;
}