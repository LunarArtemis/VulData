int uev_init1(uev_ctx_t *ctx, int maxevents)
{
	if (!ctx || maxevents < 1) {
		errno = EINVAL;
		return -1;
	}

	memset(ctx, 0, sizeof(*ctx));
	ctx->maxevents = maxevents;

	return _init(ctx, 0);
}