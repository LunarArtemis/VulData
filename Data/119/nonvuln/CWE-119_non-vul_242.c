static int modify_raw_packet_qp(struct mlx5_ib_dev *dev, struct mlx5_ib_qp *qp,
				const struct mlx5_modify_raw_qp_param *raw_qp_param,
				u8 tx_affinity)
{
	struct mlx5_ib_raw_packet_qp *raw_packet_qp = &qp->raw_packet_qp;
	struct mlx5_ib_rq *rq = &raw_packet_qp->rq;
	struct mlx5_ib_sq *sq = &raw_packet_qp->sq;
	int modify_rq = !!qp->rq.wqe_cnt;
	int modify_sq = !!qp->sq.wqe_cnt;
	int rq_state;
	int sq_state;
	int err;

	switch (raw_qp_param->operation) {
	case MLX5_CMD_OP_RST2INIT_QP:
		rq_state = MLX5_RQC_STATE_RDY;
		sq_state = MLX5_SQC_STATE_RDY;
		break;
	case MLX5_CMD_OP_2ERR_QP:
		rq_state = MLX5_RQC_STATE_ERR;
		sq_state = MLX5_SQC_STATE_ERR;
		break;
	case MLX5_CMD_OP_2RST_QP:
		rq_state = MLX5_RQC_STATE_RST;
		sq_state = MLX5_SQC_STATE_RST;
		break;
	case MLX5_CMD_OP_RTR2RTS_QP:
	case MLX5_CMD_OP_RTS2RTS_QP:
		if (raw_qp_param->set_mask ==
		    MLX5_RAW_QP_RATE_LIMIT) {
			modify_rq = 0;
			sq_state = sq->state;
		} else {
			return raw_qp_param->set_mask ? -EINVAL : 0;
		}
		break;
	case MLX5_CMD_OP_INIT2INIT_QP:
	case MLX5_CMD_OP_INIT2RTR_QP:
		if (raw_qp_param->set_mask)
			return -EINVAL;
		else
			return 0;
	default:
		WARN_ON(1);
		return -EINVAL;
	}

	if (modify_rq) {
		err =  modify_raw_packet_qp_rq(dev, rq, rq_state, raw_qp_param);
		if (err)
			return err;
	}

	if (modify_sq) {
		if (tx_affinity) {
			err = modify_raw_packet_tx_affinity(dev->mdev, sq,
							    tx_affinity);
			if (err)
				return err;
		}

		return modify_raw_packet_qp_sq(dev->mdev, sq, sq_state, raw_qp_param);
	}

	return 0;
}