static int iwlagn_send_sta_key(struct iwl_priv *priv,
			       struct ieee80211_key_conf *keyconf,
			       u8 sta_id, u32 tkip_iv32, u16 *tkip_p1k,
			       u32 cmd_flags)
{
	unsigned long flags;
	__le16 key_flags;
	struct iwl_addsta_cmd sta_cmd;
	int i;

	spin_lock_irqsave(&priv->shrd->sta_lock, flags);
	memcpy(&sta_cmd, &priv->stations[sta_id].sta, sizeof(sta_cmd));
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags);

	key_flags = cpu_to_le16(keyconf->keyidx << STA_KEY_FLG_KEYID_POS);
	key_flags |= STA_KEY_FLG_MAP_KEY_MSK;

	switch (keyconf->cipher) {
	case WLAN_CIPHER_SUITE_CCMP:
		key_flags |= STA_KEY_FLG_CCMP;
		memcpy(sta_cmd.key.key, keyconf->key, keyconf->keylen);
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		key_flags |= STA_KEY_FLG_TKIP;
		sta_cmd.key.tkip_rx_tsc_byte2 = tkip_iv32;
		for (i = 0; i < 5; i++)
			sta_cmd.key.tkip_rx_ttak[i] = cpu_to_le16(tkip_p1k[i]);
		memcpy(sta_cmd.key.key, keyconf->key, keyconf->keylen);
		break;
	case WLAN_CIPHER_SUITE_WEP104:
		key_flags |= STA_KEY_FLG_KEY_SIZE_MSK;
		/* fall through */
	case WLAN_CIPHER_SUITE_WEP40:
		key_flags |= STA_KEY_FLG_WEP;
		memcpy(&sta_cmd.key.key[3], keyconf->key, keyconf->keylen);
		break;
	default:
		WARN_ON(1);
		return -EINVAL;
	}

	if (!(keyconf->flags & IEEE80211_KEY_FLAG_PAIRWISE))
		key_flags |= STA_KEY_MULTICAST_MSK;

	/* key pointer (offset) */
	sta_cmd.key.key_offset = keyconf->hw_key_idx;

	sta_cmd.key.key_flags = key_flags;
	sta_cmd.mode = STA_CONTROL_MODIFY_MSK;
	sta_cmd.sta.modify_mask = STA_MODIFY_KEY_MASK;

	return iwl_send_add_sta(priv, &sta_cmd, cmd_flags);
}