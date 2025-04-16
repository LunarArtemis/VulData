static int iwl_send_static_wepkey_cmd(struct iwl_priv *priv,
				      struct iwl_rxon_context *ctx,
				      bool send_if_empty)
{
	int i, not_empty = 0;
	u8 buff[sizeof(struct iwl_wep_cmd) +
		sizeof(struct iwl_wep_key) * WEP_KEYS_MAX];
	struct iwl_wep_cmd *wep_cmd = (struct iwl_wep_cmd *)buff;
	size_t cmd_size  = sizeof(struct iwl_wep_cmd);
	struct iwl_host_cmd cmd = {
		.id = ctx->wep_key_cmd,
		.data = { wep_cmd, },
		.flags = CMD_SYNC,
	};

	might_sleep();

	memset(wep_cmd, 0, cmd_size +
			(sizeof(struct iwl_wep_key) * WEP_KEYS_MAX));

	for (i = 0; i < WEP_KEYS_MAX ; i++) {
		wep_cmd->key[i].key_index = i;
		if (ctx->wep_keys[i].key_size) {
			wep_cmd->key[i].key_offset = i;
			not_empty = 1;
		} else {
			wep_cmd->key[i].key_offset = WEP_INVALID_OFFSET;
		}

		wep_cmd->key[i].key_size = ctx->wep_keys[i].key_size;
		memcpy(&wep_cmd->key[i].key[3], ctx->wep_keys[i].key,
				ctx->wep_keys[i].key_size);
	}

	wep_cmd->global_key_type = WEP_KEY_WEP_TYPE;
	wep_cmd->num_keys = WEP_KEYS_MAX;

	cmd_size += sizeof(struct iwl_wep_key) * WEP_KEYS_MAX;

	cmd.len[0] = cmd_size;

	if (not_empty || send_if_empty)
		return iwl_trans_send_cmd(trans(priv), &cmd);
	else
		return 0;
}