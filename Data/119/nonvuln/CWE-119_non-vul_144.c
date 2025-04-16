static int create_raw_packet_qp_tir(struct mlx5_ib_dev *dev,
				    struct mlx5_ib_rq *rq, u32 tdn,
				    bool tunnel_offload_en)
{
	u32 *in;
	void *tirc;
	int inlen;
	int err;

	inlen = MLX5_ST_SZ_BYTES(create_tir_in);
	in = kvzalloc(inlen, GFP_KERNEL);
	if (!in)
		return -ENOMEM;

	tirc = MLX5_ADDR_OF(create_tir_in, in, ctx);
	MLX5_SET(tirc, tirc, disp_type, MLX5_TIRC_DISP_TYPE_DIRECT);
	MLX5_SET(tirc, tirc, inline_rqn, rq->base.mqp.qpn);
	MLX5_SET(tirc, tirc, transport_domain, tdn);
	if (tunnel_offload_en)
		MLX5_SET(tirc, tirc, tunneled_offload_en, 1);

	if (dev->rep)
		MLX5_SET(tirc, tirc, self_lb_block,
			 MLX5_TIRC_SELF_LB_BLOCK_BLOCK_UNICAST_);

	err = mlx5_core_create_tir(dev->mdev, in, inlen, &rq->tirn);

	kvfree(in);

	return err;
}