static int to_mlx5_st(enum ib_qp_type type)
{
	switch (type) {
	case IB_QPT_RC:			return MLX5_QP_ST_RC;
	case IB_QPT_UC:			return MLX5_QP_ST_UC;
	case IB_QPT_UD:			return MLX5_QP_ST_UD;
	case MLX5_IB_QPT_REG_UMR:	return MLX5_QP_ST_REG_UMR;
	case IB_QPT_XRC_INI:
	case IB_QPT_XRC_TGT:		return MLX5_QP_ST_XRC;
	case IB_QPT_SMI:		return MLX5_QP_ST_QP0;
	case MLX5_IB_QPT_HW_GSI:	return MLX5_QP_ST_QP1;
	case MLX5_IB_QPT_DCI:		return MLX5_QP_ST_DCI;
	case IB_QPT_RAW_IPV6:		return MLX5_QP_ST_RAW_IPV6;
	case IB_QPT_RAW_PACKET:
	case IB_QPT_RAW_ETHERTYPE:	return MLX5_QP_ST_RAW_ETHERTYPE;
	case IB_QPT_MAX:
	default:		return -EINVAL;
	}
}