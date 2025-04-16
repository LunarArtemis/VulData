int iwlagn_add_bssid_station(struct iwl_priv *priv,
			     struct iwl_rxon_context *ctx,
			     const u8 *addr, u8 *sta_id_r)
{
	int ret;
	u8 sta_id;
	struct iwl_link_quality_cmd *link_cmd;
	unsigned long flags;

	if (sta_id_r)
		*sta_id_r = IWL_INVALID_STATION;

	ret = iwl_add_station_common(priv, ctx, addr, 0, NULL, &sta_id);
	if (ret) {
		IWL_ERR(priv, "Unable to add station %pM\n", addr);
		return ret;
	}

	if (sta_id_r)
		*sta_id_r = sta_id;

	spin_lock_irqsave(&priv->shrd->sta_lock, flags);
	priv->stations[sta_id].used |= IWL_STA_LOCAL;
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

	/* Set up default rate scaling table in device's station table */
	link_cmd = iwl_sta_alloc_lq(priv, ctx, sta_id);
	if (!link_cmd) {
		IWL_ERR(priv,
			"Unable to initialize rate scaling for station %pM.\n",
			addr);
		return -ENOMEM;
	}

	ret = iwl_send_lq_cmd(priv, ctx, link_cmd, CMD_SYNC, true);
	if (ret)
		IWL_ERR(priv, "Link quality command failed (%d)\n", ret);

	spin_lock_irqsave(&priv->shrd->sta_lock, flags);
	priv->stations[sta_id].lq = link_cmd;
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

	return 0;
}