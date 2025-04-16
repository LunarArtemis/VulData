static int tcm_loop_device_reset(struct scsi_cmnd *sc)
{
	struct se_cmd *se_cmd = NULL;
	struct se_portal_group *se_tpg;
	struct se_session *se_sess;
	struct tcm_loop_cmd *tl_cmd = NULL;
	struct tcm_loop_hba *tl_hba;
	struct tcm_loop_nexus *tl_nexus;
	struct tcm_loop_tmr *tl_tmr = NULL;
	struct tcm_loop_tpg *tl_tpg;
	int ret = FAILED;
	/*
	 * Locate the tcm_loop_hba_t pointer
	 */
	tl_hba = *(struct tcm_loop_hba **)shost_priv(sc->device->host);
	/*
	 * Locate the tl_nexus and se_sess pointers
	 */
	tl_nexus = tl_hba->tl_nexus;
	if (!tl_nexus) {
		printk(KERN_ERR "Unable to perform device reset without"
				" active I_T Nexus\n");
		return FAILED;
	}
	se_sess = tl_nexus->se_sess;
	/*
	 * Locate the tl_tpg and se_tpg pointers from TargetID in sc->device->id
	 */
	tl_tpg = &tl_hba->tl_hba_tpgs[sc->device->id];
	se_tpg = &tl_tpg->tl_se_tpg;

	tl_cmd = kmem_cache_zalloc(tcm_loop_cmd_cache, GFP_KERNEL);
	if (!tl_cmd) {
		printk(KERN_ERR "Unable to allocate memory for tl_cmd\n");
		return FAILED;
	}

	tl_tmr = kzalloc(sizeof(struct tcm_loop_tmr), GFP_KERNEL);
	if (!tl_tmr) {
		printk(KERN_ERR "Unable to allocate memory for tl_tmr\n");
		goto release;
	}
	init_waitqueue_head(&tl_tmr->tl_tmr_wait);

	se_cmd = &tl_cmd->tl_se_cmd;
	/*
	 * Initialize struct se_cmd descriptor from target_core_mod infrastructure
	 */
	transport_init_se_cmd(se_cmd, se_tpg->se_tpg_tfo, se_sess, 0,
				DMA_NONE, MSG_SIMPLE_TAG,
				&tl_cmd->tl_sense_buf[0]);
	/*
	 * Allocate the LUN_RESET TMR
	 */
	se_cmd->se_tmr_req = core_tmr_alloc_req(se_cmd, tl_tmr,
				TMR_LUN_RESET);
	if (IS_ERR(se_cmd->se_tmr_req))
		goto release;
	/*
	 * Locate the underlying TCM struct se_lun from sc->device->lun
	 */
	if (transport_lookup_tmr_lun(se_cmd, sc->device->lun) < 0)
		goto release;
	/*
	 * Queue the TMR to TCM Core and sleep waiting for tcm_loop_queue_tm_rsp()
	 * to wake us up.
	 */
	transport_generic_handle_tmr(se_cmd);
	wait_event(tl_tmr->tl_tmr_wait, atomic_read(&tl_tmr->tmr_complete));
	/*
	 * The TMR LUN_RESET has completed, check the response status and
	 * then release allocations.
	 */
	ret = (se_cmd->se_tmr_req->response == TMR_FUNCTION_COMPLETE) ?
		SUCCESS : FAILED;
release:
	if (se_cmd)
		transport_generic_free_cmd(se_cmd, 1, 0);
	else
		kmem_cache_free(tcm_loop_cmd_cache, tl_cmd);
	kfree(tl_tmr);
	return ret;
}