static int __begin_wqe(struct mlx5_ib_qp *qp, void **seg,
		     struct mlx5_wqe_ctrl_seg **ctrl,
		     const struct ib_send_wr *wr, unsigned *idx,
		     int *size, int nreq, bool send_signaled, bool solicited)
{
	if (unlikely(mlx5_wq_overflow(&qp->sq, nreq, qp->ibqp.send_cq)))
		return -ENOMEM;

	*idx = qp->sq.cur_post & (qp->sq.wqe_cnt - 1);
	*seg = mlx5_get_send_wqe(qp, *idx);
	*ctrl = *seg;
	*(uint32_t *)(*seg + 8) = 0;
	(*ctrl)->imm = send_ieth(wr);
	(*ctrl)->fm_ce_se = qp->sq_signal_bits |
		(send_signaled ? MLX5_WQE_CTRL_CQ_UPDATE : 0) |
		(solicited ? MLX5_WQE_CTRL_SOLICITED : 0);

	*seg += sizeof(**ctrl);
	*size = sizeof(**ctrl) / 16;

	return 0;
}