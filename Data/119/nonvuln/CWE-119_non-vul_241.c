static int modify_raw_packet_tx_affinity(struct mlx5_core_dev *dev,
					 struct mlx5_ib_sq *sq, u8 tx_affinity)
{
	void *in;
	void *tisc;
	int inlen;
	int err;

	inlen = MLX5_ST_SZ_BYTES(modify_tis_in);
	in = kvzalloc(inlen, GFP_KERNEL);
	if (!in)
		return -ENOMEM;

	MLX5_SET(modify_tis_in, in, bitmask.lag_tx_port_affinity, 1);

	tisc = MLX5_ADDR_OF(modify_tis_in, in, ctx);
	MLX5_SET(tisc, tisc, lag_tx_port_affinity, tx_affinity);

	err = mlx5_core_modify_tis(dev, sq->tisn, in, inlen);

	kvfree(in);

	return err;
}