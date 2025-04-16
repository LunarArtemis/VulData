int iwl_send_add_sta(struct iwl_priv *priv,
		     struct iwl_addsta_cmd *sta, u8 flags)
{
	int ret = 0;
	struct iwl_host_cmd cmd = {
		.id = REPLY_ADD_STA,
		.flags = flags,
		.data = { sta, },
		.len = { sizeof(*sta), },
	};
	u8 sta_id __maybe_unused = sta->sta.sta_id;

	IWL_DEBUG_INFO(priv, "Adding sta %u (%pM) %ssynchronously\n",
		       sta_id, sta->sta.addr, flags & CMD_ASYNC ?  "a" : "");

	if (!(flags & CMD_ASYNC)) {
		cmd.flags |= CMD_WANT_SKB;
		might_sleep();
	}

	ret = iwl_trans_send_cmd(trans(priv), &cmd);

	if (ret || (flags & CMD_ASYNC))
		return ret;
	/*else the command was successfully sent in SYNC mode, need to free
	 * the reply page */

	iwl_free_pages(priv->shrd, cmd.reply_page);

	if (cmd.handler_status)
		IWL_ERR(priv, "%s - error in the CMD response %d", __func__,
			cmd.handler_status);

	return cmd.handler_status;
}