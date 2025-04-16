static int tcm_loop_is_state_remove(struct se_cmd *se_cmd)
{
	/*
	 * Assume struct scsi_cmnd is not in remove state..
	 */
	return 0;
}