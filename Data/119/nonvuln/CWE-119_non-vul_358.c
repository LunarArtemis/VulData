int iwl_remove_default_wep_key(struct iwl_priv *priv,
			       struct iwl_rxon_context *ctx,
			       struct ieee80211_key_conf *keyconf)
{
	int ret;

	lockdep_assert_held(&priv->shrd->mutex);

	IWL_DEBUG_WEP(priv, "Removing default WEP key: idx=%d\n",
		      keyconf->keyidx);

	memset(&ctx->wep_keys[keyconf->keyidx], 0, sizeof(ctx->wep_keys[0]));
	if (iwl_is_rfkill(priv->shrd)) {
		IWL_DEBUG_WEP(priv,
			"Not sending REPLY_WEPKEY command due to RFKILL.\n");
		/* but keys in device are clear anyway so return success */
		return 0;
	}
	ret = iwl_send_static_wepkey_cmd(priv, ctx, 1);
	IWL_DEBUG_WEP(priv, "Remove default WEP key: idx=%d ret=%d\n",
		      keyconf->keyidx, ret);

	return ret;
}