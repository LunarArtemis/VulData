static int sq_overhead(struct ib_qp_init_attr *attr)
{
	int size = 0;

	switch (attr->qp_type) {
	case IB_QPT_XRC_INI:
		size += sizeof(struct mlx5_wqe_xrc_seg);
		/* fall through */
	case IB_QPT_RC:
		size += sizeof(struct mlx5_wqe_ctrl_seg) +
			max(sizeof(struct mlx5_wqe_atomic_seg) +
			    sizeof(struct mlx5_wqe_raddr_seg),
			    sizeof(struct mlx5_wqe_umr_ctrl_seg) +
			    sizeof(struct mlx5_mkey_seg) +
			    MLX5_IB_SQ_UMR_INLINE_THRESHOLD /
			    MLX5_IB_UMR_OCTOWORD);
		break;

	case IB_QPT_XRC_TGT:
		return 0;

	case IB_QPT_UC:
		size += sizeof(struct mlx5_wqe_ctrl_seg) +
			max(sizeof(struct mlx5_wqe_raddr_seg),
			    sizeof(struct mlx5_wqe_umr_ctrl_seg) +
			    sizeof(struct mlx5_mkey_seg));
		break;

	case IB_QPT_UD:
		if (attr->create_flags & IB_QP_CREATE_IPOIB_UD_LSO)
			size += sizeof(struct mlx5_wqe_eth_pad) +
				sizeof(struct mlx5_wqe_eth_seg);
		/* fall through */
	case IB_QPT_SMI:
	case MLX5_IB_QPT_HW_GSI:
		size += sizeof(struct mlx5_wqe_ctrl_seg) +
			sizeof(struct mlx5_wqe_datagram_seg);
		break;

	case MLX5_IB_QPT_REG_UMR:
		size += sizeof(struct mlx5_wqe_ctrl_seg) +
			sizeof(struct mlx5_wqe_umr_ctrl_seg) +
			sizeof(struct mlx5_mkey_seg);
		break;

	default:
		return -EINVAL;
	}

	return size;
}