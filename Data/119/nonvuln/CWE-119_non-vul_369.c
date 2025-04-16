int iwlagn_alloc_bcast_station(struct iwl_priv *priv,
			       struct iwl_rxon_context *ctx)
{
	struct iwl_link_quality_cmd *link_cmd;
	unsigned long flags;
	u8 sta_id;

	spin_lock_irqsave(&priv->shrd->sta_lock, flags);
	sta_id = iwl_prep_station(priv, ctx, iwl_bcast_addr, false, NULL);
	if (sta_id == IWL_INVALID_STATION) {
		IWL_ERR(priv, "Unable to prepare broadcast station\n");
		spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

		return -EINVAL;
	}

	priv->stations[sta_id].used |= IWL_STA_DRIVER_ACTIVE;
	priv->stations[sta_id].used |= IWL_STA_BCAST;
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

	link_cmd = iwl_sta_alloc_lq(priv, ctx, sta_id);
	if (!link_cmd) {
		IWL_ERR(priv,
			"Unable to initialize rate scaling for bcast station.\n");
		return -ENOMEM;
	}

	spin_lock_irqsave(&priv->shrd->sta_lock, flags);
	priv->stations[sta_id].lq = link_cmd;
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

	return 0;
}