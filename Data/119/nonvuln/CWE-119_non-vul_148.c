static void get_cqs(enum ib_qp_type qp_type,
		    struct ib_cq *ib_send_cq, struct ib_cq *ib_recv_cq,
		    struct mlx5_ib_cq **send_cq, struct mlx5_ib_cq **recv_cq)
{
	switch (qp_type) {
	case IB_QPT_XRC_TGT:
		*send_cq = NULL;
		*recv_cq = NULL;
		break;
	case MLX5_IB_QPT_REG_UMR:
	case IB_QPT_XRC_INI:
		*send_cq = ib_send_cq ? to_mcq(ib_send_cq) : NULL;
		*recv_cq = NULL;
		break;

	case IB_QPT_SMI:
	case MLX5_IB_QPT_HW_GSI:
	case IB_QPT_RC:
	case IB_QPT_UC:
	case IB_QPT_UD:
	case IB_QPT_RAW_IPV6:
	case IB_QPT_RAW_ETHERTYPE:
	case IB_QPT_RAW_PACKET:
		*send_cq = ib_send_cq ? to_mcq(ib_send_cq) : NULL;
		*recv_cq = ib_recv_cq ? to_mcq(ib_recv_cq) : NULL;
		break;

	case IB_QPT_MAX:
	default:
		*send_cq = NULL;
		*recv_cq = NULL;
		break;
	}
}