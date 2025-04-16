static inline enum ib_qp_state to_ib_qp_state(enum mlx5_qp_state mlx5_state)
{
	switch (mlx5_state) {
	case MLX5_QP_STATE_RST:      return IB_QPS_RESET;
	case MLX5_QP_STATE_INIT:     return IB_QPS_INIT;
	case MLX5_QP_STATE_RTR:      return IB_QPS_RTR;
	case MLX5_QP_STATE_RTS:      return IB_QPS_RTS;
	case MLX5_QP_STATE_SQ_DRAINING:
	case MLX5_QP_STATE_SQD:      return IB_QPS_SQD;
	case MLX5_QP_STATE_SQER:     return IB_QPS_SQE;
	case MLX5_QP_STATE_ERR:      return IB_QPS_ERR;
	default:		     return -1;
	}
}