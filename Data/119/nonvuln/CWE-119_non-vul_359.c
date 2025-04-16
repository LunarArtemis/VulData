int iwl_sta_tx_modify_enable_tid(struct iwl_priv *priv, int sta_id, int tid)
{
	unsigned long flags;
	struct iwl_addsta_cmd sta_cmd;

	lockdep_assert_held(&priv->shrd->mutex);

	/* Remove "disable" flag, to enable Tx for this TID */
	spin_lock_irqsave(&priv->shrd->sta_lock, flags);
	priv->stations[sta_id].sta.sta.modify_mask = STA_MODIFY_TID_DISABLE_TX;
	priv->stations[sta_id].sta.tid_disable_tx &= cpu_to_le16(~(1 << tid));
	priv->stations[sta_id].sta.mode = STA_CONTROL_MODIFY_MSK;
	memcpy(&sta_cmd, &priv->stations[sta_id].sta, sizeof(struct iwl_addsta_cmd));
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

	return iwl_send_add_sta(priv, &sta_cmd, CMD_SYNC);
}