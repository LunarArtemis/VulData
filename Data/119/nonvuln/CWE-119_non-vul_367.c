void iwl_update_tkip_key(struct iwl_priv *priv,
			 struct ieee80211_vif *vif,
			 struct ieee80211_key_conf *keyconf,
			 struct ieee80211_sta *sta, u32 iv32, u16 *phase1key)
{
	u8 sta_id = iwlagn_key_sta_id(priv, vif, sta);

	if (sta_id == IWL_INVALID_STATION)
		return;

	if (iwl_scan_cancel(priv)) {
		/* cancel scan failed, just live w/ bad key and rely
		   briefly on SW decryption */
		return;
	}

	iwlagn_send_sta_key(priv, keyconf, sta_id,
			    iv32, phase1key, CMD_ASYNC);
}