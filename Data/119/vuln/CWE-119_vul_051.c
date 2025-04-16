static int devlink_param_get(struct devlink *devlink,
			     const struct devlink_param *param,
			     struct devlink_param_gset_ctx *ctx)
{
	if (!param->get)
		return -EOPNOTSUPP;
	return param->get(devlink, param->id, ctx);
}