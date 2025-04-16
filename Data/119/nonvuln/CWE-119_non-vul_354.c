int iwl_add_sta_callback(struct iwl_priv *priv, struct iwl_rx_mem_buffer *rxb,
			       struct iwl_device_cmd *cmd)
{
	struct iwl_rx_packet *pkt = rxb_addr(rxb);
	struct iwl_addsta_cmd *addsta =
		(struct iwl_addsta_cmd *) cmd->payload;

	return iwl_process_add_sta_resp(priv, addsta, pkt);
}