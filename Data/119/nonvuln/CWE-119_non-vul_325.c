static struct se_cmd *tcm_loop_allocate_core_cmd(
	struct tcm_loop_hba *tl_hba,
	struct se_portal_group *se_tpg,
	struct scsi_cmnd *sc)
{
	struct se_cmd *se_cmd;
	struct se_session *se_sess;
	struct tcm_loop_nexus *tl_nexus = tl_hba->tl_nexus;
	struct tcm_loop_cmd *tl_cmd;
	int sam_task_attr;

	if (!tl_nexus) {
		scmd_printk(KERN_ERR, sc, "TCM_Loop I_T Nexus"
				" does not exist\n");
		set_host_byte(sc, DID_ERROR);
		return NULL;
	}
	se_sess = tl_nexus->se_sess;

	tl_cmd = kmem_cache_zalloc(tcm_loop_cmd_cache, GFP_ATOMIC);
	if (!tl_cmd) {
		printk(KERN_ERR "Unable to allocate struct tcm_loop_cmd\n");
		set_host_byte(sc, DID_ERROR);
		return NULL;
	}
	se_cmd = &tl_cmd->tl_se_cmd;
	/*
	 * Save the pointer to struct scsi_cmnd *sc
	 */
	tl_cmd->sc = sc;
	/*
	 * Locate the SAM Task Attr from struct scsi_cmnd *
	 */
	if (sc->device->tagged_supported) {
		switch (sc->tag) {
		case HEAD_OF_QUEUE_TAG:
			sam_task_attr = MSG_HEAD_TAG;
			break;
		case ORDERED_QUEUE_TAG:
			sam_task_attr = MSG_ORDERED_TAG;
			break;
		default:
			sam_task_attr = MSG_SIMPLE_TAG;
			break;
		}
	} else
		sam_task_attr = MSG_SIMPLE_TAG;

	/*
	 * Initialize struct se_cmd descriptor from target_core_mod infrastructure
	 */
	transport_init_se_cmd(se_cmd, se_tpg->se_tpg_tfo, se_sess,
			scsi_bufflen(sc), sc->sc_data_direction, sam_task_attr,
			&tl_cmd->tl_sense_buf[0]);

	/*
	 * Signal BIDI usage with T_TASK(cmd)->t_tasks_bidi
	 */
	if (scsi_bidi_cmnd(sc))
		se_cmd->t_tasks_bidi = 1;
	/*
	 * Locate the struct se_lun pointer and attach it to struct se_cmd
	 */
	if (transport_lookup_cmd_lun(se_cmd, tl_cmd->sc->device->lun) < 0) {
		kmem_cache_free(tcm_loop_cmd_cache, tl_cmd);
		set_host_byte(sc, DID_NO_CONNECT);
		return NULL;
	}

	return se_cmd;
}