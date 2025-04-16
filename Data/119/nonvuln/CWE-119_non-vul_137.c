static int  create_rq(struct mlx5_ib_rwq *rwq, struct ib_pd *pd,
		      struct ib_wq_init_attr *init_attr)
{
	struct mlx5_ib_dev *dev;
	int has_net_offloads;
	__be64 *rq_pas0;
	void *in;
	void *rqc;
	void *wq;
	int inlen;
	int err;

	dev = to_mdev(pd->device);

	inlen = MLX5_ST_SZ_BYTES(create_rq_in) + sizeof(u64) * rwq->rq_num_pas;
	in = kvzalloc(inlen, GFP_KERNEL);
	if (!in)
		return -ENOMEM;

	rqc = MLX5_ADDR_OF(create_rq_in, in, ctx);
	MLX5_SET(rqc,  rqc, mem_rq_type,
		 MLX5_RQC_MEM_RQ_TYPE_MEMORY_RQ_INLINE);
	MLX5_SET(rqc, rqc, user_index, rwq->user_index);
	MLX5_SET(rqc,  rqc, cqn, to_mcq(init_attr->cq)->mcq.cqn);
	MLX5_SET(rqc,  rqc, state, MLX5_RQC_STATE_RST);
	MLX5_SET(rqc,  rqc, flush_in_error_en, 1);
	wq = MLX5_ADDR_OF(rqc, rqc, wq);
	MLX5_SET(wq, wq, wq_type,
		 rwq->create_flags & MLX5_IB_WQ_FLAGS_STRIDING_RQ ?
		 MLX5_WQ_TYPE_CYCLIC_STRIDING_RQ : MLX5_WQ_TYPE_CYCLIC);
	if (init_attr->create_flags & IB_WQ_FLAGS_PCI_WRITE_END_PADDING) {
		if (!MLX5_CAP_GEN(dev->mdev, end_pad)) {
			mlx5_ib_dbg(dev, "Scatter end padding is not supported\n");
			err = -EOPNOTSUPP;
			goto out;
		} else {
			MLX5_SET(wq, wq, end_padding_mode, MLX5_WQ_END_PAD_MODE_ALIGN);
		}
	}
	MLX5_SET(wq, wq, log_wq_stride, rwq->log_rq_stride);
	if (rwq->create_flags & MLX5_IB_WQ_FLAGS_STRIDING_RQ) {
		MLX5_SET(wq, wq, two_byte_shift_en, rwq->two_byte_shift_en);
		MLX5_SET(wq, wq, log_wqe_stride_size,
			 rwq->single_stride_log_num_of_bytes -
			 MLX5_MIN_SINGLE_STRIDE_LOG_NUM_BYTES);
		MLX5_SET(wq, wq, log_wqe_num_of_strides, rwq->log_num_strides -
			 MLX5_MIN_SINGLE_WQE_LOG_NUM_STRIDES);
	}
	MLX5_SET(wq, wq, log_wq_sz, rwq->log_rq_size);
	MLX5_SET(wq, wq, pd, to_mpd(pd)->pdn);
	MLX5_SET(wq, wq, page_offset, rwq->rq_page_offset);
	MLX5_SET(wq, wq, log_wq_pg_sz, rwq->log_page_size);
	MLX5_SET(wq, wq, wq_signature, rwq->wq_sig);
	MLX5_SET64(wq, wq, dbr_addr, rwq->db.dma);
	has_net_offloads = MLX5_CAP_GEN(dev->mdev, eth_net_offloads);
	if (init_attr->create_flags & IB_WQ_FLAGS_CVLAN_STRIPPING) {
		if (!(has_net_offloads && MLX5_CAP_ETH(dev->mdev, vlan_cap))) {
			mlx5_ib_dbg(dev, "VLAN offloads are not supported\n");
			err = -EOPNOTSUPP;
			goto out;
		}
	} else {
		MLX5_SET(rqc, rqc, vsd, 1);
	}
	if (init_attr->create_flags & IB_WQ_FLAGS_SCATTER_FCS) {
		if (!(has_net_offloads && MLX5_CAP_ETH(dev->mdev, scatter_fcs))) {
			mlx5_ib_dbg(dev, "Scatter FCS is not supported\n");
			err = -EOPNOTSUPP;
			goto out;
		}
		MLX5_SET(rqc, rqc, scatter_fcs, 1);
	}
	if (init_attr->create_flags & IB_WQ_FLAGS_DELAY_DROP) {
		if (!(dev->ib_dev.attrs.raw_packet_caps &
		      IB_RAW_PACKET_CAP_DELAY_DROP)) {
			mlx5_ib_dbg(dev, "Delay drop is not supported\n");
			err = -EOPNOTSUPP;
			goto out;
		}
		MLX5_SET(rqc, rqc, delay_drop_en, 1);
	}
	rq_pas0 = (__be64 *)MLX5_ADDR_OF(wq, wq, pas);
	mlx5_ib_populate_pas(dev, rwq->umem, rwq->page_shift, rq_pas0, 0);
	err = mlx5_core_create_rq_tracked(dev->mdev, in, inlen, &rwq->core_qp);
	if (!err && init_attr->create_flags & IB_WQ_FLAGS_DELAY_DROP) {
		err = set_delay_drop(dev);
		if (err) {
			mlx5_ib_warn(dev, "Failed to enable delay drop err=%d\n",
				     err);
			mlx5_core_destroy_rq_tracked(dev->mdev, &rwq->core_qp);
		} else {
			rwq->create_flags |= MLX5_IB_WQ_FLAGS_DELAY_DROP;
		}
	}
out:
	kvfree(in);
	return err;
}