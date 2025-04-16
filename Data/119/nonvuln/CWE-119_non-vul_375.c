static u8 iwlagn_key_sta_id(struct iwl_priv *priv,
			    struct ieee80211_vif *vif,
			    struct ieee80211_sta *sta)
{
	struct iwl_vif_priv *vif_priv = (void *)vif->drv_priv;
	u8 sta_id = IWL_INVALID_STATION;

	if (sta)
		sta_id = iwl_sta_id(sta);

	/*
	 * The device expects GTKs for station interfaces to be
	 * installed as GTKs for the AP station. If we have no
	 * station ID, then use the ap_sta_id in that case.
	 */
	if (!sta && vif && vif_priv->ctx) {
		switch (vif->type) {
		case NL80211_IFTYPE_STATION:
			sta_id = vif_priv->ctx->ap_sta_id;
			break;
		default:
			/*
			 * In all other cases, the key will be
			 * used either for TX only or is bound
			 * to a station already.
			 */
			break;
		}
	}

	return sta_id;
}