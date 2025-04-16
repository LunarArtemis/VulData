int iwl_restore_default_wep_keys(struct iwl_priv *priv,
				 struct iwl_rxon_context *ctx)
{
	lockdep_assert_held(&priv->shrd->mutex);

	return iwl_send_static_wepkey_cmd(priv, ctx, false);
}