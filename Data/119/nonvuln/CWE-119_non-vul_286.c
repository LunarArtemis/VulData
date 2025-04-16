static int tcm_loop_get_cmd_state(struct se_cmd *se_cmd)
{
	struct tcm_loop_cmd *tl_cmd = container_of(se_cmd,
			struct tcm_loop_cmd, tl_se_cmd);

	return tl_cmd->sc_cmd_state;
}