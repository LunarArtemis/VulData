static struct ib_qp *mlx5_ib_create_dct(struct ib_pd *pd,
					struct ib_qp_init_attr *attr,
					struct mlx5_ib_create_qp *ucmd)
{
	struct mlx5_ib_qp *qp;
	int err = 0;
	u32 uidx = MLX5_IB_DEFAULT_UIDX;
	void *dctc;

	if (!attr->srq || !attr->recv_cq)
		return ERR_PTR(-EINVAL);

	err = get_qp_user_index(to_mucontext(pd->uobject->context),
				ucmd, sizeof(*ucmd), &uidx);
	if (err)
		return ERR_PTR(err);

	qp = kzalloc(sizeof(*qp), GFP_KERNEL);
	if (!qp)
		return ERR_PTR(-ENOMEM);

	qp->dct.in = kzalloc(MLX5_ST_SZ_BYTES(create_dct_in), GFP_KERNEL);
	if (!qp->dct.in) {
		err = -ENOMEM;
		goto err_free;
	}

	dctc = MLX5_ADDR_OF(create_dct_in, qp->dct.in, dct_context_entry);
	qp->qp_sub_type = MLX5_IB_QPT_DCT;
	MLX5_SET(dctc, dctc, pd, to_mpd(pd)->pdn);
	MLX5_SET(dctc, dctc, srqn_xrqn, to_msrq(attr->srq)->msrq.srqn);
	MLX5_SET(dctc, dctc, cqn, to_mcq(attr->recv_cq)->mcq.cqn);
	MLX5_SET64(dctc, dctc, dc_access_key, ucmd->access_key);
	MLX5_SET(dctc, dctc, user_index, uidx);

	qp->state = IB_QPS_RESET;

	return &qp->ibqp;
err_free:
	kfree(qp);
	return ERR_PTR(err);
}