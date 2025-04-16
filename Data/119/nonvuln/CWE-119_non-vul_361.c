void iwl_sta_fill_lq(struct iwl_priv *priv, struct iwl_rxon_context *ctx,
		     u8 sta_id, struct iwl_link_quality_cmd *link_cmd)
{
	int i, r;
	u32 rate_flags = 0;
	__le32 rate_n_flags;

	lockdep_assert_held(&priv->shrd->mutex);

	memset(link_cmd, 0, sizeof(*link_cmd));

	/* Set up the rate scaling to start at selected rate, fall back
	 * all the way down to 1M in IEEE order, and then spin on 1M */
	if (priv->band == IEEE80211_BAND_5GHZ)
		r = IWL_RATE_6M_INDEX;
	else if (ctx && ctx->vif && ctx->vif->p2p)
		r = IWL_RATE_6M_INDEX;
	else
		r = IWL_RATE_1M_INDEX;

	if (r >= IWL_FIRST_CCK_RATE && r <= IWL_LAST_CCK_RATE)
		rate_flags |= RATE_MCS_CCK_MSK;

	rate_flags |= first_antenna(hw_params(priv).valid_tx_ant) <<
				RATE_MCS_ANT_POS;
	rate_n_flags = iwl_hw_set_rate_n_flags(iwl_rates[r].plcp, rate_flags);
	for (i = 0; i < LINK_QUAL_MAX_RETRY_NUM; i++)
		link_cmd->rs_table[i].rate_n_flags = rate_n_flags;

	link_cmd->general_params.single_stream_ant_msk =
			first_antenna(hw_params(priv).valid_tx_ant);

	link_cmd->general_params.dual_stream_ant_msk =
		hw_params(priv).valid_tx_ant &
		~first_antenna(hw_params(priv).valid_tx_ant);
	if (!link_cmd->general_params.dual_stream_ant_msk) {
		link_cmd->general_params.dual_stream_ant_msk = ANT_AB;
	} else if (num_of_ant(hw_params(priv).valid_tx_ant) == 2) {
		link_cmd->general_params.dual_stream_ant_msk =
			hw_params(priv).valid_tx_ant;
	}

	link_cmd->agg_params.agg_dis_start_th =
		LINK_QUAL_AGG_DISABLE_START_DEF;
	link_cmd->agg_params.agg_time_limit =
		cpu_to_le16(LINK_QUAL_AGG_TIME_LIMIT_DEF);

	link_cmd->sta_id = sta_id;
}