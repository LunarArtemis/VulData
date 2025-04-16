static int sqrq_state_to_qp_state(u8 sq_state, u8 rq_state,
				  struct mlx5_ib_qp *qp, u8 *qp_state)
{
	static const u8 sqrq_trans[MLX5_RQ_NUM_STATE][MLX5_SQ_NUM_STATE] = {
		[MLX5_RQC_STATE_RST] = {
			[MLX5_SQC_STATE_RST]	= IB_QPS_RESET,
			[MLX5_SQC_STATE_RDY]	= MLX5_QP_STATE_BAD,
			[MLX5_SQC_STATE_ERR]	= MLX5_QP_STATE_BAD,
			[MLX5_SQ_STATE_NA]	= IB_QPS_RESET,
		},
		[MLX5_RQC_STATE_RDY] = {
			[MLX5_SQC_STATE_RST]	= MLX5_QP_STATE_BAD,
			[MLX5_SQC_STATE_RDY]	= MLX5_QP_STATE,
			[MLX5_SQC_STATE_ERR]	= IB_QPS_SQE,
			[MLX5_SQ_STATE_NA]	= MLX5_QP_STATE,
		},
		[MLX5_RQC_STATE_ERR] = {
			[MLX5_SQC_STATE_RST]    = MLX5_QP_STATE_BAD,
			[MLX5_SQC_STATE_RDY]	= MLX5_QP_STATE_BAD,
			[MLX5_SQC_STATE_ERR]	= IB_QPS_ERR,
			[MLX5_SQ_STATE_NA]	= IB_QPS_ERR,
		},
		[MLX5_RQ_STATE_NA] = {
			[MLX5_SQC_STATE_RST]    = IB_QPS_RESET,
			[MLX5_SQC_STATE_RDY]	= MLX5_QP_STATE,
			[MLX5_SQC_STATE_ERR]	= MLX5_QP_STATE,
			[MLX5_SQ_STATE_NA]	= MLX5_QP_STATE_BAD,
		},
	};

	*qp_state = sqrq_trans[rq_state][sq_state];

	if (*qp_state == MLX5_QP_STATE_BAD) {
		WARN(1, "Buggy Raw Packet QP state, SQ 0x%x state: 0x%x, RQ 0x%x state: 0x%x",
		     qp->raw_packet_qp.sq.base.mqp.qpn, sq_state,
		     qp->raw_packet_qp.rq.base.mqp.qpn, rq_state);
		return -EINVAL;
	}

	if (*qp_state == MLX5_QP_STATE)
		*qp_state = qp->state;

	return 0;
}