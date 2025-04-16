static void tcm_loop_check_stop_free(struct se_cmd *se_cmd)
{
	/*
	 * Do not release struct se_cmd's containing a valid TMR
	 * pointer.  These will be released directly in tcm_loop_device_reset()
	 * with transport_generic_free_cmd().
	 */
	if (se_cmd->se_tmr_req)
		return;
	/*
	 * Release the struct se_cmd, which will make a callback to release
	 * struct tcm_loop_cmd * in tcm_loop_deallocate_core_cmd()
	 */
	transport_generic_free_cmd(se_cmd, 0, 0);
}