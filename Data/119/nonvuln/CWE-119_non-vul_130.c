static int modify_raw_packet_qp_rq(struct mlx5_ib_dev *dev,
				   struct mlx5_ib_rq *rq, int new_state,
				   const struct mlx5_modify_raw_qp_param *raw_qp_param)
{
	void *in;
	void *rqc;
	int inlen;
	int err;

	inlen = MLX5_ST_SZ_BYTES(modify_rq_in);
	in = kvzalloc(inlen, GFP_KERNEL);
	if (!in)
		return -ENOMEM;

	MLX5_SET(modify_rq_in, in, rq_state, rq->state);

	rqc = MLX5_ADDR_OF(modify_rq_in, in, ctx);
	MLX5_SET(rqc, rqc, state, new_state);

	if (raw_qp_param->set_mask & MLX5_RAW_QP_MOD_SET_RQ_Q_CTR_ID) {
		if (MLX5_CAP_GEN(dev->mdev, modify_rq_counter_set_id)) {
			MLX5_SET64(modify_rq_in, in, modify_bitmask,
				   MLX5_MODIFY_RQ_IN_MODIFY_BITMASK_RQ_COUNTER_SET_ID);
			MLX5_SET(rqc, rqc, counter_set_id, raw_qp_param->rq_q_ctr_id);
		} else
			pr_info_once("%s: RAW PACKET QP counters are not supported on current FW\n",
				     dev->ib_dev.name);
	}

	err = mlx5_core_modify_rq(dev->mdev, rq->base.mqp.qpn, in, inlen);
	if (err)
		goto out;

	rq->state = new_state;

out:
	kvfree(in);
	return err;
}