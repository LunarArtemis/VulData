static int is_connected(enum ib_qp_type qp_type)
{
	if (qp_type == IB_QPT_RC || qp_type == IB_QPT_UC)
		return 1;

	return 0;
}