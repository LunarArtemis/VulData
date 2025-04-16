static int create_kernel_qp(struct mlx5_ib_dev *dev,
			    struct ib_qp_init_attr *init_attr,
			    struct mlx5_ib_qp *qp,
			    u32 **in, int *inlen,
			    struct mlx5_ib_qp_base *base)
{
	int uar_index;
	void *qpc;
	int err;

	if (init_attr->create_flags & ~(IB_QP_CREATE_SIGNATURE_EN |
					IB_QP_CREATE_BLOCK_MULTICAST_LOOPBACK |
					IB_QP_CREATE_IPOIB_UD_LSO |
					IB_QP_CREATE_NETIF_QP |
					mlx5_ib_create_qp_sqpn_qp1()))
		return -EINVAL;

	if (init_attr->qp_type == MLX5_IB_QPT_REG_UMR)
		qp->bf.bfreg = &dev->fp_bfreg;
	else
		qp->bf.bfreg = &dev->bfreg;

	/* We need to divide by two since each register is comprised of
	 * two buffers of identical size, namely odd and even
	 */
	qp->bf.buf_size = (1 << MLX5_CAP_GEN(dev->mdev, log_bf_reg_size)) / 2;
	uar_index = qp->bf.bfreg->index;

	err = calc_sq_size(dev, init_attr, qp);
	if (err < 0) {
		mlx5_ib_dbg(dev, "err %d\n", err);
		return err;
	}

	qp->rq.offset = 0;
	qp->sq.offset = qp->rq.wqe_cnt << qp->rq.wqe_shift;
	base->ubuffer.buf_size = err + (qp->rq.wqe_cnt << qp->rq.wqe_shift);

	err = mlx5_buf_alloc(dev->mdev, base->ubuffer.buf_size, &qp->buf);
	if (err) {
		mlx5_ib_dbg(dev, "err %d\n", err);
		return err;
	}

	qp->sq.qend = mlx5_get_send_wqe(qp, qp->sq.wqe_cnt);
	*inlen = MLX5_ST_SZ_BYTES(create_qp_in) +
		 MLX5_FLD_SZ_BYTES(create_qp_in, pas[0]) * qp->buf.npages;
	*in = kvzalloc(*inlen, GFP_KERNEL);
	if (!*in) {
		err = -ENOMEM;
		goto err_buf;
	}

	qpc = MLX5_ADDR_OF(create_qp_in, *in, qpc);
	MLX5_SET(qpc, qpc, uar_page, uar_index);
	MLX5_SET(qpc, qpc, log_page_size, qp->buf.page_shift - MLX5_ADAPTER_PAGE_SHIFT);

	/* Set "fast registration enabled" for all kernel QPs */
	MLX5_SET(qpc, qpc, fre, 1);
	MLX5_SET(qpc, qpc, rlky, 1);

	if (init_attr->create_flags & mlx5_ib_create_qp_sqpn_qp1()) {
		MLX5_SET(qpc, qpc, deth_sqpn, 1);
		qp->flags |= MLX5_IB_QP_SQPN_QP1;
	}

	mlx5_fill_page_array(&qp->buf,
			     (__be64 *)MLX5_ADDR_OF(create_qp_in, *in, pas));

	err = mlx5_db_alloc(dev->mdev, &qp->db);
	if (err) {
		mlx5_ib_dbg(dev, "err %d\n", err);
		goto err_free;
	}

	qp->sq.wrid = kvmalloc_array(qp->sq.wqe_cnt,
				     sizeof(*qp->sq.wrid), GFP_KERNEL);
	qp->sq.wr_data = kvmalloc_array(qp->sq.wqe_cnt,
					sizeof(*qp->sq.wr_data), GFP_KERNEL);
	qp->rq.wrid = kvmalloc_array(qp->rq.wqe_cnt,
				     sizeof(*qp->rq.wrid), GFP_KERNEL);
	qp->sq.w_list = kvmalloc_array(qp->sq.wqe_cnt,
				       sizeof(*qp->sq.w_list), GFP_KERNEL);
	qp->sq.wqe_head = kvmalloc_array(qp->sq.wqe_cnt,
					 sizeof(*qp->sq.wqe_head), GFP_KERNEL);

	if (!qp->sq.wrid || !qp->sq.wr_data || !qp->rq.wrid ||
	    !qp->sq.w_list || !qp->sq.wqe_head) {
		err = -ENOMEM;
		goto err_wrid;
	}
	qp->create_type = MLX5_QP_KERNEL;

	return 0;

err_wrid:
	kvfree(qp->sq.wqe_head);
	kvfree(qp->sq.w_list);
	kvfree(qp->sq.wrid);
	kvfree(qp->sq.wr_data);
	kvfree(qp->rq.wrid);
	mlx5_db_free(dev->mdev, &qp->db);

err_free:
	kvfree(*in);

err_buf:
	mlx5_buf_free(dev->mdev, &qp->buf);
	return err;
}