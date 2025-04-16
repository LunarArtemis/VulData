int iwl_set_default_wep_key(struct iwl_priv *priv,
			    struct iwl_rxon_context *ctx,
			    struct ieee80211_key_conf *keyconf)
{
	int ret;

	lockdep_assert_held(&priv->shrd->mutex);

	if (keyconf->keylen != WEP_KEY_LEN_128 &&
	    keyconf->keylen != WEP_KEY_LEN_64) {
		IWL_DEBUG_WEP(priv,
			      "Bad WEP key length %d\n", keyconf->keylen);
		return -EINVAL;
	}

	keyconf->hw_key_idx = IWLAGN_HW_KEY_DEFAULT;

	ctx->wep_keys[keyconf->keyidx].key_size = keyconf->keylen;
	memcpy(&ctx->wep_keys[keyconf->keyidx].key, &keyconf->key,
							keyconf->keylen);

	ret = iwl_send_static_wepkey_cmd(priv, ctx, false);
	IWL_DEBUG_WEP(priv, "Set default WEP key: len=%d idx=%d ret=%d\n",
		keyconf->keylen, keyconf->keyidx, ret);

	return ret;
}