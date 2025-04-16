void iwl_clear_ucode_stations(struct iwl_priv *priv,
			      struct iwl_rxon_context *ctx)
{
	int i;
	unsigned long flags_spin;
	bool cleared = false;

	IWL_DEBUG_INFO(priv, "Clearing ucode stations in driver\n");

	spin_lock_irqsave(&priv->shrd->sta_lock, flags_spin);
	for (i = 0; i < IWLAGN_STATION_COUNT; i++) {
		if (ctx && ctx->ctxid != priv->stations[i].ctxid)
			continue;

		if (priv->stations[i].used & IWL_STA_UCODE_ACTIVE) {
			IWL_DEBUG_INFO(priv,
				"Clearing ucode active for station %d\n", i);
			priv->stations[i].used &= ~IWL_STA_UCODE_ACTIVE;
			cleared = true;
		}
	}
	spin_unlock_irqrestore(&priv->shrd->sta_lock, flags_spin);

	if (!cleared)
		IWL_DEBUG_INFO(priv,
			       "No active stations found to be cleared\n");
}