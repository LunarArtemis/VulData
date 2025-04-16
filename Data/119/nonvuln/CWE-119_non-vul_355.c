int iwl_update_bcast_station(struct iwl_priv *priv,
			     struct iwl_rxon_context *ctx)
{
	unsigned long flags;
	struct iwl_link_quality_cmd *link_cmd;
	u8 sta_id = ctx->bcast_sta_id;

	link_cmd = iwl_sta_alloc_lq(priv, ctx, sta_id);
	if (!link_cmd) {
		IWL_ERR(priv, "Unable to initialize rate scaling for bcast station.\n");
		return -ENOMEM;
	}

	spin_lock_irqsave(&priv->shrd->sta_lock, flags);
	if (priv->stations[sta_id].lq)
		kfree(priv->stations[sta_id].lq);
	else
		IWL_DEBUG_INFO(priv, "Bcast station rate scaling has not been initialized yet.\n");
	priv->stations[sta_id].lq = link_cmd;
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

	return 0;
}