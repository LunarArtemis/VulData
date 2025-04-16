int iwl_sta_rx_agg_stop(struct iwl_priv *priv, struct ieee80211_sta *sta,
			int tid)
{
	unsigned long flags;
	int sta_id;
	struct iwl_addsta_cmd sta_cmd;

	lockdep_assert_held(&priv->shrd->mutex);

	sta_id = iwl_sta_id(sta);
	if (sta_id == IWL_INVALID_STATION) {
		IWL_ERR(priv, "Invalid station for AGG tid %d\n", tid);
		return -ENXIO;
	}

	spin_lock_irqsave(&priv->shrd->sta_lock, flags);
	priv->stations[sta_id].sta.station_flags_msk = 0;
	priv->stations[sta_id].sta.sta.modify_mask = STA_MODIFY_DELBA_TID_MSK;
	priv->stations[sta_id].sta.remove_immediate_ba_tid = (u8)tid;
	priv->stations[sta_id].sta.mode = STA_CONTROL_MODIFY_MSK;
	memcpy(&sta_cmd, &priv->stations[sta_id].sta, sizeof(struct iwl_addsta_cmd));
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

	return iwl_send_add_sta(priv, &sta_cmd, CMD_SYNC);
}