struct ib_rwq_ind_table *mlx5_ib_create_rwq_ind_table(struct ib_device *device,
						      struct ib_rwq_ind_table_init_attr *init_attr,
						      struct ib_udata *udata)
{
	struct mlx5_ib_dev *dev = to_mdev(device);
	struct mlx5_ib_rwq_ind_table *rwq_ind_tbl;
	int sz = 1 << init_attr->log_ind_tbl_size;
	struct mlx5_ib_create_rwq_ind_tbl_resp resp = {};
	size_t min_resp_len;
	int inlen;
	int err;
	int i;
	u32 *in;
	void *rqtc;

	if (udata->inlen > 0 &&
	    !ib_is_udata_cleared(udata, 0,
				 udata->inlen))
		return ERR_PTR(-EOPNOTSUPP);

	if (init_attr->log_ind_tbl_size >
	    MLX5_CAP_GEN(dev->mdev, log_max_rqt_size)) {
		mlx5_ib_dbg(dev, "log_ind_tbl_size = %d is bigger than supported = %d\n",
			    init_attr->log_ind_tbl_size,
			    MLX5_CAP_GEN(dev->mdev, log_max_rqt_size));
		return ERR_PTR(-EINVAL);
	}

	min_resp_len = offsetof(typeof(resp), reserved) + sizeof(resp.reserved);
	if (udata->outlen && udata->outlen < min_resp_len)
		return ERR_PTR(-EINVAL);

	rwq_ind_tbl = kzalloc(sizeof(*rwq_ind_tbl), GFP_KERNEL);
	if (!rwq_ind_tbl)
		return ERR_PTR(-ENOMEM);

	inlen = MLX5_ST_SZ_BYTES(create_rqt_in) + sizeof(u32) * sz;
	in = kvzalloc(inlen, GFP_KERNEL);
	if (!in) {
		err = -ENOMEM;
		goto err;
	}

	rqtc = MLX5_ADDR_OF(create_rqt_in, in, rqt_context);

	MLX5_SET(rqtc, rqtc, rqt_actual_size, sz);
	MLX5_SET(rqtc, rqtc, rqt_max_size, sz);

	for (i = 0; i < sz; i++)
		MLX5_SET(rqtc, rqtc, rq_num[i], init_attr->ind_tbl[i]->wq_num);

	err = mlx5_core_create_rqt(dev->mdev, in, inlen, &rwq_ind_tbl->rqtn);
	kvfree(in);

	if (err)
		goto err;

	rwq_ind_tbl->ib_rwq_ind_tbl.ind_tbl_num = rwq_ind_tbl->rqtn;
	if (udata->outlen) {
		resp.response_length = offsetof(typeof(resp), response_length) +
					sizeof(resp.response_length);
		err = ib_copy_to_udata(udata, &resp, resp.response_length);
		if (err)
			goto err_copy;
	}

	return &rwq_ind_tbl->ib_rwq_ind_tbl;

err_copy:
	mlx5_core_destroy_rqt(dev->mdev, rwq_ind_tbl->rqtn);
err:
	kfree(rwq_ind_tbl);
	return ERR_PTR(err);
}