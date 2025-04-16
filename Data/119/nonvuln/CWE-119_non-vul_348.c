void iwl_restore_stations(struct iwl_priv *priv, struct iwl_rxon_context *ctx)
{
	struct iwl_addsta_cmd sta_cmd;
	struct iwl_link_quality_cmd lq;
	unsigned long flags_spin;
	int i;
	bool found = false;
	int ret;
	bool send_lq;

	if (!iwl_is_ready(priv->shrd)) {
		IWL_DEBUG_INFO(priv,
			       "Not ready yet, not restoring any stations.\n");
		return;
	}

	IWL_DEBUG_ASSOC(priv, "Restoring all known stations ... start.\n");
	spin_lock_irqsave(&priv->shrd->sta_lock, flags_spin);
	for (i = 0; i < IWLAGN_STATION_COUNT; i++) {
		if (ctx->ctxid != priv->stations[i].ctxid)
			continue;
		if ((priv->stations[i].used & IWL_STA_DRIVER_ACTIVE) &&
			    !(priv->stations[i].used & IWL_STA_UCODE_ACTIVE)) {
			IWL_DEBUG_ASSOC(priv, "Restoring sta %pM\n",
					priv->stations[i].sta.sta.addr);
			priv->stations[i].sta.mode = 0;
			priv->stations[i].used |= IWL_STA_UCODE_INPROGRESS;
			found = true;
		}
	}

	for (i = 0; i < IWLAGN_STATION_COUNT; i++) {
		if ((priv->stations[i].used & IWL_STA_UCODE_INPROGRESS)) {
			memcpy(&sta_cmd, &priv->stations[i].sta,
			       sizeof(struct iwl_addsta_cmd));
			send_lq = false;
			if (priv->stations[i].lq) {
				if (priv->shrd->wowlan)
					iwl_sta_fill_lq(priv, ctx, i, &lq);
				else
					memcpy(&lq, priv->stations[i].lq,
					       sizeof(struct iwl_link_quality_cmd));
				send_lq = true;
			}
			spin_unlock_irqrestore(&priv->shrd->sta_lock,
					       flags_spin);
			ret = iwl_send_add_sta(priv, &sta_cmd, CMD_SYNC);
			if (ret) {
				spin_lock_irqsave(&priv->shrd->sta_lock,
						  flags_spin);
				IWL_ERR(priv, "Adding station %pM failed.\n",
					priv->stations[i].sta.sta.addr);
				priv->stations[i].used &=
						~IWL_STA_DRIVER_ACTIVE;
				priv->stations[i].used &=
						~IWL_STA_UCODE_INPROGRESS;
				spin_unlock_irqrestore(&priv->shrd->sta_lock,
						       flags_spin);
			}
			/*
			 * Rate scaling has already been initialized, send
			 * current LQ command
			 */
			if (send_lq)
				iwl_send_lq_cmd(priv, ctx, &lq,
						CMD_SYNC, true);
			spin_lock_irqsave(&priv->shrd->sta_lock, flags_spin);
			priv->stations[i].used &= ~IWL_STA_UCODE_INPROGRESS;
		}
	}

	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags_spin);
	if (!found)
		IWL_DEBUG_INFO(priv, "Restoring all known stations .... "
			"no stations to be restored.\n");
	else
		IWL_DEBUG_INFO(priv, "Restoring all known stations .... "
			"complete.\n");
}