static int tcm_loop_queue_tm_rsp(struct se_cmd *se_cmd)
{
	struct se_tmr_req *se_tmr = se_cmd->se_tmr_req;
	struct tcm_loop_tmr *tl_tmr = se_tmr->fabric_tmr_ptr;
	/*
	 * The SCSI EH thread will be sleeping on se_tmr->tl_tmr_wait, go ahead
	 * and wake up the wait_queue_head_t in tcm_loop_device_reset()
	 */
	atomic_set(&tl_tmr->tmr_complete, 1);
	wake_up(&tl_tmr->tl_tmr_wait);
	return 0;
}