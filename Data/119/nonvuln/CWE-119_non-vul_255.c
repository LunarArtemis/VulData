static int query_raw_packet_qp_sq_state(struct mlx5_ib_dev *dev,
					struct mlx5_ib_sq *sq,
					u8 *sq_state)
{
	int err;

	err = mlx5_core_query_sq_state(dev->mdev, sq->base.mqp.qpn, sq_state);
	if (err)
		goto out;
	sq->state = *sq_state;

out:
	return err;
}