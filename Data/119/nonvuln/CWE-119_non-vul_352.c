static int iwl_send_remove_station(struct iwl_priv *priv,
				   const u8 *addr, int sta_id,
				   bool temporary)
{
	struct iwl_rx_packet *pkt;
	int ret;

	unsigned long flags_spin;
	struct iwl_rem_sta_cmd rm_sta_cmd;

	struct iwl_host_cmd cmd = {
		.id = REPLY_REMOVE_STA,
		.len = { sizeof(struct iwl_rem_sta_cmd), },
		.flags = CMD_SYNC,
		.data = { &rm_sta_cmd, },
	};

	memset(&rm_sta_cmd, 0, sizeof(rm_sta_cmd));
	rm_sta_cmd.num_sta = 1;
	memcpy(&rm_sta_cmd.addr, addr, ETH_ALEN);

	cmd.flags |= CMD_WANT_SKB;

	ret = iwl_trans_send_cmd(trans(priv), &cmd);

	if (ret)
		return ret;

	pkt = (struct iwl_rx_packet *)cmd.reply_page;
	if (pkt->hdr.flags & IWL_CMD_FAILED_MSK) {
		IWL_ERR(priv, "Bad return from REPLY_REMOVE_STA (0x%08X)\n",
			  pkt->hdr.flags);
		ret = -EIO;
	}

	if (!ret) {
		switch (pkt->u.rem_sta.status) {
		case REM_STA_SUCCESS_MSK:
			if (!temporary) {
				spin_lock_irqsave(&priv->shrd->sta_lock,
					flags_spin);
				iwl_sta_ucode_deactivate(priv, sta_id);
				spin_unlock_irqrestore(&priv->shrd->sta_lock,
					flags_spin);
			}
			IWL_DEBUG_ASSOC(priv, "REPLY_REMOVE_STA PASSED\n");
			break;
		default:
			ret = -EIO;
			IWL_ERR(priv, "REPLY_REMOVE_STA failed\n");
			break;
		}
	}
	iwl_free_pages(priv->shrd, cmd.reply_page);

	return ret;
}