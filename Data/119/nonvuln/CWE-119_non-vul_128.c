static bool modify_dci_qp_is_ok(enum ib_qp_state cur_state, enum ib_qp_state new_state,
				enum ib_qp_attr_mask attr_mask)
{
	int req = IB_QP_STATE;
	int opt = 0;

	if (cur_state == IB_QPS_RESET && new_state == IB_QPS_INIT) {
		req |= IB_QP_PKEY_INDEX | IB_QP_PORT;
		return is_valid_mask(attr_mask, req, opt);
	} else if (cur_state == IB_QPS_INIT && new_state == IB_QPS_INIT) {
		opt = IB_QP_PKEY_INDEX | IB_QP_PORT;
		return is_valid_mask(attr_mask, req, opt);
	} else if (cur_state == IB_QPS_INIT && new_state == IB_QPS_RTR) {
		req |= IB_QP_PATH_MTU;
		opt = IB_QP_PKEY_INDEX;
		return is_valid_mask(attr_mask, req, opt);
	} else if (cur_state == IB_QPS_RTR && new_state == IB_QPS_RTS) {
		req |= IB_QP_TIMEOUT | IB_QP_RETRY_CNT | IB_QP_RNR_RETRY |
		       IB_QP_MAX_QP_RD_ATOMIC | IB_QP_SQ_PSN;
		opt = IB_QP_MIN_RNR_TIMER;
		return is_valid_mask(attr_mask, req, opt);
	} else if (cur_state == IB_QPS_RTS && new_state == IB_QPS_RTS) {
		opt = IB_QP_MIN_RNR_TIMER;
		return is_valid_mask(attr_mask, req, opt);
	} else if (cur_state != IB_QPS_RESET && new_state == IB_QPS_ERR) {
		return is_valid_mask(attr_mask, req, opt);
	}
	return false;
}