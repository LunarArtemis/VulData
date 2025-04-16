void iwl_reprogram_ap_sta(struct iwl_priv *priv, struct iwl_rxon_context *ctx)
{
	unsigned long flags;
	int sta_id = ctx->ap_sta_id;
	int ret;
	struct iwl_addsta_cmd sta_cmd;
	struct iwl_link_quality_cmd lq;
	bool active, have_lq = false;

	spin_lock_irqsave(&priv->shrd->sta_lock, flags);
	if (!(priv->stations[sta_id].used & IWL_STA_DRIVER_ACTIVE)) {
		spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);
		return;
	}

	memcpy(&sta_cmd, &priv->stations[sta_id].sta, sizeof(sta_cmd));
	sta_cmd.mode = 0;
	if (priv->stations[sta_id].lq) {
		memcpy(&lq, priv->stations[sta_id].lq, sizeof(lq));
		have_lq = true;
	}

	active = priv->stations[sta_id].used & IWL_STA_UCODE_ACTIVE;
	priv->stations[sta_id].used &= ~IWL_STA_DRIVER_ACTIVE;
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

	if (active) {
		ret = iwl_send_remove_station(
			priv, priv->stations[sta_id].sta.sta.addr,
			sta_id, true);
		if (ret)
			IWL_ERR(priv, "failed to remove STA %pM (%d)\n",
				priv->stations[sta_id].sta.sta.addr, ret);
	}
	spin_lock_irqsave(&priv->shrd->sta_lock, flags);
	priv->stations[sta_id].used |= IWL_STA_DRIVER_ACTIVE;
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

	ret = iwl_send_add_sta(priv, &sta_cmd, CMD_SYNC);
	if (ret)
		IWL_ERR(priv, "failed to re-add STA %pM (%d)\n",
			priv->stations[sta_id].sta.sta.addr, ret);
	if (have_lq)
		iwl_send_lq_cmd(priv, ctx, &lq, CMD_SYNC, true);
}