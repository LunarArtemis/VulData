void iwl_sta_modify_sleep_tx_count(struct iwl_priv *priv, int sta_id, int cnt)
{
	unsigned long flags;

	spin_lock_irqsave(&priv->shrd->sta_lock, flags);
	priv->stations[sta_id].sta.station_flags |= STA_FLG_PWR_SAVE_MSK;
	priv->stations[sta_id].sta.station_flags_msk = STA_FLG_PWR_SAVE_MSK;
	priv->stations[sta_id].sta.sta.modify_mask =
					STA_MODIFY_SLEEP_TX_COUNT_MSK;
	priv->stations[sta_id].sta.sleep_tx_count = cpu_to_le16(cnt);
	priv->stations[sta_id].sta.mode = STA_CONTROL_MODIFY_MSK;
	iwl_send_add_sta(priv, &priv->stations[sta_id].sta, CMD_ASYNC);
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

}