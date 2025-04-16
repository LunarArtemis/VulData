int iwl_set_dynamic_key(struct iwl_priv *priv,
			struct iwl_rxon_context *ctx,
			struct ieee80211_key_conf *keyconf,
			struct ieee80211_sta *sta)
{
	struct ieee80211_key_seq seq;
	u16 p1k[5];
	int ret;
	u8 sta_id = iwlagn_key_sta_id(priv, ctx->vif, sta);
	const u8 *addr;

	if (sta_id == IWL_INVALID_STATION)
		return -EINVAL;

	lockdep_assert_held(&priv->shrd->mutex);

	keyconf->hw_key_idx = iwl_get_free_ucode_key_offset(priv);
	if (keyconf->hw_key_idx == WEP_INVALID_OFFSET)
		return -ENOSPC;

	ctx->key_mapping_keys++;

	switch (keyconf->cipher) {
	case WLAN_CIPHER_SUITE_TKIP:
		if (sta)
			addr = sta->addr;
		else /* station mode case only */
			addr = ctx->active.bssid_addr;

		/* pre-fill phase 1 key into device cache */
		ieee80211_get_key_rx_seq(keyconf, 0, &seq);
		ieee80211_get_tkip_rx_p1k(keyconf, addr, seq.tkip.iv32, p1k);
		ret = iwlagn_send_sta_key(priv, keyconf, sta_id,
					  seq.tkip.iv32, p1k, CMD_SYNC);
		break;
	case WLAN_CIPHER_SUITE_CCMP:
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		ret = iwlagn_send_sta_key(priv, keyconf, sta_id,
					  0, NULL, CMD_SYNC);
		break;
	default:
		IWL_ERR(priv, "Unknown cipher %x\n", keyconf->cipher);
		ret = -EINVAL;
	}

	if (ret) {
		ctx->key_mapping_keys--;
		clear_bit(keyconf->hw_key_idx, &priv->ucode_key_table);
	}

	IWL_DEBUG_WEP(priv, "Set dynamic key: cipher=%x len=%d idx=%d sta=%pM ret=%d\n",
		      keyconf->cipher, keyconf->keylen, keyconf->keyidx,
		      sta ? sta->addr : NULL, ret);

	return ret;
}