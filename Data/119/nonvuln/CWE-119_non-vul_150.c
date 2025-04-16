static int create_raw_packet_qp_sq(struct mlx5_ib_dev *dev,
				   struct mlx5_ib_sq *sq, void *qpin,
				   struct ib_pd *pd)
{
	struct mlx5_ib_ubuffer *ubuffer = &sq->ubuffer;
	__be64 *pas;
	void *in;
	void *sqc;
	void *qpc = MLX5_ADDR_OF(create_qp_in, qpin, qpc);
	void *wq;
	int inlen;
	int err;
	int page_shift = 0;
	int npages;
	int ncont = 0;
	u32 offset = 0;

	err = mlx5_ib_umem_get(dev, pd, ubuffer->buf_addr, ubuffer->buf_size,
			       &sq->ubuffer.umem, &npages, &page_shift,
			       &ncont, &offset);
	if (err)
		return err;

	inlen = MLX5_ST_SZ_BYTES(create_sq_in) + sizeof(u64) * ncont;
	in = kvzalloc(inlen, GFP_KERNEL);
	if (!in) {
		err = -ENOMEM;
		goto err_umem;
	}

	sqc = MLX5_ADDR_OF(create_sq_in, in, ctx);
	MLX5_SET(sqc, sqc, flush_in_error_en, 1);
	if (MLX5_CAP_ETH(dev->mdev, multi_pkt_send_wqe))
		MLX5_SET(sqc, sqc, allow_multi_pkt_send_wqe, 1);
	MLX5_SET(sqc, sqc, state, MLX5_SQC_STATE_RST);
	MLX5_SET(sqc, sqc, user_index, MLX5_GET(qpc, qpc, user_index));
	MLX5_SET(sqc, sqc, cqn, MLX5_GET(qpc, qpc, cqn_snd));
	MLX5_SET(sqc, sqc, tis_lst_sz, 1);
	MLX5_SET(sqc, sqc, tis_num_0, sq->tisn);
	if (MLX5_CAP_GEN(dev->mdev, eth_net_offloads) &&
	    MLX5_CAP_ETH(dev->mdev, swp))
		MLX5_SET(sqc, sqc, allow_swp, 1);

	wq = MLX5_ADDR_OF(sqc, sqc, wq);
	MLX5_SET(wq, wq, wq_type, MLX5_WQ_TYPE_CYCLIC);
	MLX5_SET(wq, wq, pd, MLX5_GET(qpc, qpc, pd));
	MLX5_SET(wq, wq, uar_page, MLX5_GET(qpc, qpc, uar_page));
	MLX5_SET64(wq, wq, dbr_addr, MLX5_GET64(qpc, qpc, dbr_addr));
	MLX5_SET(wq, wq, log_wq_stride, ilog2(MLX5_SEND_WQE_BB));
	MLX5_SET(wq, wq, log_wq_sz, MLX5_GET(qpc, qpc, log_sq_size));
	MLX5_SET(wq, wq, log_wq_pg_sz,  page_shift - MLX5_ADAPTER_PAGE_SHIFT);
	MLX5_SET(wq, wq, page_offset, offset);

	pas = (__be64 *)MLX5_ADDR_OF(wq, wq, pas);
	mlx5_ib_populate_pas(dev, sq->ubuffer.umem, page_shift, pas, 0);

	err = mlx5_core_create_sq_tracked(dev->mdev, in, inlen, &sq->base.mqp);

	kvfree(in);

	if (err)
		goto err_umem;

	err = create_flow_rule_vport_sq(dev, sq);
	if (err)
		goto err_flow;

	return 0;

err_flow:
	mlx5_core_destroy_sq_tracked(dev->mdev, &sq->base.mqp);

err_umem:
	ib_umem_release(sq->ubuffer.umem);
	sq->ubuffer.umem = NULL;

	return err;
}