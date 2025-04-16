int iwl_remove_dynamic_key(struct iwl_priv *priv,
			   struct iwl_rxon_context *ctx,
			   struct ieee80211_key_conf *keyconf,
			   struct ieee80211_sta *sta)
{
	unsigned long flags;
	struct iwl_addsta_cmd sta_cmd;
	u8 sta_id = iwlagn_key_sta_id(priv, ctx->vif, sta);

	/* if station isn't there, neither is the key */
	if (sta_id == IWL_INVALID_STATION)
		return -ENOENT;

	spin_lock_irqsave(&priv->shrd->sta_lock, flags);
	memcpy(&sta_cmd, &priv->stations[sta_id].sta, sizeof(sta_cmd));
	if (!(priv->stations[sta_id].used & IWL_STA_UCODE_ACTIVE))
		sta_id = IWL_INVALID_STATION;
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

	if (sta_id == IWL_INVALID_STATION)
		return 0;

	lockdep_assert_held(&priv->shrd->mutex);

	ctx->key_mapping_keys--;

	IWL_DEBUG_WEP(priv, "Remove dynamic key: idx=%d sta=%d\n",
		      keyconf->keyidx, sta_id);

	if (!test_and_clear_bit(keyconf->hw_key_idx, &priv->ucode_key_table))
		IWL_ERR(priv, "offset %d not used in uCode key table.\n",
			keyconf->hw_key_idx);

	sta_cmd.key.key_flags = STA_KEY_FLG_NO_ENC | STA_KEY_FLG_INVALID;
	sta_cmd.key.key_offset = WEP_INVALID_OFFSET;
	sta_cmd.sta.modify_mask = STA_MODIFY_KEY_MASK;
	sta_cmd.mode = STA_CONTROL_MODIFY_MSK;

	return iwl_send_add_sta(priv, &sta_cmd, CMD_SYNC);
}