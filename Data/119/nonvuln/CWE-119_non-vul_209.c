static int query_raw_packet_qp_state(struct mlx5_ib_dev *dev,
				     struct mlx5_ib_qp *qp,
				     u8 *raw_packet_qp_state)
{
	struct mlx5_ib_raw_packet_qp *raw_packet_qp = &qp->raw_packet_qp;
	struct mlx5_ib_sq *sq = &raw_packet_qp->sq;
	struct mlx5_ib_rq *rq = &raw_packet_qp->rq;
	int err;
	u8 sq_state = MLX5_SQ_STATE_NA;
	u8 rq_state = MLX5_RQ_STATE_NA;

	if (qp->sq.wqe_cnt) {
		err = query_raw_packet_qp_sq_state(dev, sq, &sq_state);
		if (err)
			return err;
	}

	if (qp->rq.wqe_cnt) {
		err = query_raw_packet_qp_rq_state(dev, rq, &rq_state);
		if (err)
			return err;
	}

	return sqrq_state_to_qp_state(sq_state, rq_state, qp,
				      raw_packet_qp_state);
}