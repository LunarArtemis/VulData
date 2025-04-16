int iwl_update_bcast_stations(struct iwl_priv *priv)
{
	struct iwl_rxon_context *ctx;
	int ret = 0;

	for_each_context(priv, ctx) {
		ret = iwl_update_bcast_station(priv, ctx);
		if (ret)
			break;
	}

	return ret;
}