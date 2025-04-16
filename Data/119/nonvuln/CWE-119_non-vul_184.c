static int modify_raw_packet_qp_sq(struct mlx5_core_dev *dev,
				   struct mlx5_ib_sq *sq,
				   int new_state,
				   const struct mlx5_modify_raw_qp_param *raw_qp_param)
{
	struct mlx5_ib_qp *ibqp = sq->base.container_mibqp;
	struct mlx5_rate_limit old_rl = ibqp->rl;
	struct mlx5_rate_limit new_rl = old_rl;
	bool new_rate_added = false;
	u16 rl_index = 0;
	void *in;
	void *sqc;
	int inlen;
	int err;

	inlen = MLX5_ST_SZ_BYTES(modify_sq_in);
	in = kvzalloc(inlen, GFP_KERNEL);
	if (!in)
		return -ENOMEM;

	MLX5_SET(modify_sq_in, in, sq_state, sq->state);

	sqc = MLX5_ADDR_OF(modify_sq_in, in, ctx);
	MLX5_SET(sqc, sqc, state, new_state);

	if (raw_qp_param->set_mask & MLX5_RAW_QP_RATE_LIMIT) {
		if (new_state != MLX5_SQC_STATE_RDY)
			pr_warn("%s: Rate limit can only be changed when SQ is moving to RDY\n",
				__func__);
		else
			new_rl = raw_qp_param->rl;
	}

	if (!mlx5_rl_are_equal(&old_rl, &new_rl)) {
		if (new_rl.rate) {
			err = mlx5_rl_add_rate(dev, &rl_index, &new_rl);
			if (err) {
				pr_err("Failed configuring rate limit(err %d): \
				       rate %u, max_burst_sz %u, typical_pkt_sz %u\n",
				       err, new_rl.rate, new_rl.max_burst_sz,
				       new_rl.typical_pkt_sz);

				goto out;
			}
			new_rate_added = true;
		}

		MLX5_SET64(modify_sq_in, in, modify_bitmask, 1);
		/* index 0 means no limit */
		MLX5_SET(sqc, sqc, packet_pacing_rate_limit_index, rl_index);
	}

	err = mlx5_core_modify_sq(dev, sq->base.mqp.qpn, in, inlen);
	if (err) {
		/* Remove new rate from table if failed */
		if (new_rate_added)
			mlx5_rl_remove_rate(dev, &new_rl);
		goto out;
	}

	/* Only remove the old rate after new rate was set */
	if ((old_rl.rate &&
	     !mlx5_rl_are_equal(&old_rl, &new_rl)) ||
	    (new_state != MLX5_SQC_STATE_RDY))
		mlx5_rl_remove_rate(dev, &old_rl);

	ibqp->rl = new_rl;
	sq->state = new_state;

out:
	kvfree(in);
	return err;
}