static void iwl_set_ht_add_station(struct iwl_priv *priv, u8 index,
				   struct ieee80211_sta *sta,
				   struct iwl_rxon_context *ctx)
{
	struct ieee80211_sta_ht_cap *sta_ht_inf = &sta->ht_cap;
	__le32 sta_flags;
	u8 mimo_ps_mode;

	if (!sta || !sta_ht_inf->ht_supported)
		goto done;

	mimo_ps_mode = (sta_ht_inf->cap & IEEE80211_HT_CAP_SM_PS) >> 2;
	IWL_DEBUG_ASSOC(priv, "spatial multiplexing power save mode: %s\n",
			(mimo_ps_mode == WLAN_HT_CAP_SM_PS_STATIC) ?
			"static" :
			(mimo_ps_mode == WLAN_HT_CAP_SM_PS_DYNAMIC) ?
			"dynamic" : "disabled");

	sta_flags = priv->stations[index].sta.station_flags;

	sta_flags &= ~(STA_FLG_RTS_MIMO_PROT_MSK | STA_FLG_MIMO_DIS_MSK);

	switch (mimo_ps_mode) {
	case WLAN_HT_CAP_SM_PS_STATIC:
		sta_flags |= STA_FLG_MIMO_DIS_MSK;
		break;
	case WLAN_HT_CAP_SM_PS_DYNAMIC:
		sta_flags |= STA_FLG_RTS_MIMO_PROT_MSK;
		break;
	case WLAN_HT_CAP_SM_PS_DISABLED:
		break;
	default:
		IWL_WARN(priv, "Invalid MIMO PS mode %d\n", mimo_ps_mode);
		break;
	}

	sta_flags |= cpu_to_le32(
	      (u32)sta_ht_inf->ampdu_factor << STA_FLG_MAX_AGG_SIZE_POS);

	sta_flags |= cpu_to_le32(
	      (u32)sta_ht_inf->ampdu_density << STA_FLG_AGG_MPDU_DENSITY_POS);

	if (iwl_is_ht40_tx_allowed(priv, ctx, &sta->ht_cap))
		sta_flags |= STA_FLG_HT40_EN_MSK;
	else
		sta_flags &= ~STA_FLG_HT40_EN_MSK;

	priv->stations[index].sta.station_flags = sta_flags;
 done:
	return;
}