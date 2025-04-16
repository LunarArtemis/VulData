static int devlink_param_set(struct devlink *devlink,
			     const struct devlink_param *param,
			     struct devlink_param_gset_ctx *ctx)
{
	if (!param->set)
		return -EOPNOTSUPP;
	return param->set(devlink, param->id, ctx);
}