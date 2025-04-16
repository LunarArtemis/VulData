static int begin_wqe(struct mlx5_ib_qp *qp, void **seg,
		     struct mlx5_wqe_ctrl_seg **ctrl,
		     const struct ib_send_wr *wr, unsigned *idx,
		     int *size, int nreq)
{
	return __begin_wqe(qp, seg, ctrl, wr, idx, size, nreq,
			   wr->send_flags & IB_SEND_SIGNALED,
			   wr->send_flags & IB_SEND_SOLICITED);
}