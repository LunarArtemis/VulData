static int is_qp0(enum ib_qp_type qp_type)
{
	return qp_type == IB_QPT_SMI;
}