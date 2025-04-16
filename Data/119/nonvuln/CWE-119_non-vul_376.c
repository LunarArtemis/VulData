int iwl_add_station_common(struct iwl_priv *priv, struct iwl_rxon_context *ctx,
			   const u8 *addr, bool is_ap,
			   struct ieee80211_sta *sta, u8 *sta_id_r)
{
	unsigned long flags_spin;
	int ret = 0;
	u8 sta_id;
	struct iwl_addsta_cmd sta_cmd;

	*sta_id_r = 0;
	spin_lock_irqsave(&priv->shrd->sta_lock, flags_spin);
	sta_id = iwl_prep_station(priv, ctx, addr, is_ap, sta);
	if (sta_id == IWL_INVALID_STATION) {
		IWL_ERR(priv, "Unable to prepare station %pM for addition\n",
			addr);
		spin_unlock_irqrestore(&priv->shrd->sta_lock, flags_spin);
		return -EINVAL;
	}

	/*
	 * uCode is not able to deal with multiple requests to add a
	 * station. Keep track if one is in progress so that we do not send
	 * another.
	 */
	if (priv->stations[sta_id].used & IWL_STA_UCODE_INPROGRESS) {
		IWL_DEBUG_INFO(priv, "STA %d already in process of being "
			       "added.\n", sta_id);
		spin_unlock_irqrestore(&priv->shrd->sta_lock, flags_spin);
		return -EEXIST;
	}

	if ((priv->stations[sta_id].used & IWL_STA_DRIVER_ACTIVE) &&
	    (priv->stations[sta_id].used & IWL_STA_UCODE_ACTIVE)) {
		IWL_DEBUG_ASSOC(priv, "STA %d (%pM) already added, not "
				"adding again.\n", sta_id, addr);
		spin_unlock_irqrestore(&priv->shrd->sta_lock, flags_spin);
		return -EEXIST;
	}

	priv->stations[sta_id].used |= IWL_STA_UCODE_INPROGRESS;
	memcpy(&sta_cmd, &priv->stations[sta_id].sta,
	       sizeof(struct iwl_addsta_cmd));
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags_spin);

	/* Add station to device's station table */
	ret = iwl_send_add_sta(priv, &sta_cmd, CMD_SYNC);
	if (ret) {
		spin_lock_irqsave(&priv->shrd->sta_lock, flags_spin);
		IWL_ERR(priv, "Adding station %pM failed.\n",
			priv->stations[sta_id].sta.sta.addr);
		priv->stations[sta_id].used &= ~IWL_STA_DRIVER_ACTIVE;
		priv->stations[sta_id].used &= ~IWL_STA_UCODE_INPROGRESS;
		spin_unlock_irqrestore(&priv->shrd->sta_lock, flags_spin);
	}
	*sta_id_r = sta_id;
	return ret;
}