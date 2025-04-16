struct ib_wq *mlx5_ib_create_wq(struct ib_pd *pd,
				struct ib_wq_init_attr *init_attr,
				struct ib_udata *udata)
{
	struct mlx5_ib_dev *dev;
	struct mlx5_ib_rwq *rwq;
	struct mlx5_ib_create_wq_resp resp = {};
	size_t min_resp_len;
	int err;

	if (!udata)
		return ERR_PTR(-ENOSYS);

	min_resp_len = offsetof(typeof(resp), reserved) + sizeof(resp.reserved);
	if (udata->outlen && udata->outlen < min_resp_len)
		return ERR_PTR(-EINVAL);

	dev = to_mdev(pd->device);
	switch (init_attr->wq_type) {
	case IB_WQT_RQ:
		rwq = kzalloc(sizeof(*rwq), GFP_KERNEL);
		if (!rwq)
			return ERR_PTR(-ENOMEM);
		err = prepare_user_rq(pd, init_attr, udata, rwq);
		if (err)
			goto err;
		err = create_rq(rwq, pd, init_attr);
		if (err)
			goto err_user_rq;
		break;
	default:
		mlx5_ib_dbg(dev, "unsupported wq type %d\n",
			    init_attr->wq_type);
		return ERR_PTR(-EINVAL);
	}

	rwq->ibwq.wq_num = rwq->core_qp.qpn;
	rwq->ibwq.state = IB_WQS_RESET;
	if (udata->outlen) {
		resp.response_length = offsetof(typeof(resp), response_length) +
				sizeof(resp.response_length);
		err = ib_copy_to_udata(udata, &resp, resp.response_length);
		if (err)
			goto err_copy;
	}

	rwq->core_qp.event = mlx5_ib_wq_event;
	rwq->ibwq.event_handler = init_attr->event_handler;
	return &rwq->ibwq;

err_copy:
	mlx5_core_destroy_rq_tracked(dev->mdev, &rwq->core_qp);
err_user_rq:
	destroy_user_rq(dev, pd, rwq);
err:
	kfree(rwq);
	return ERR_PTR(err);
}