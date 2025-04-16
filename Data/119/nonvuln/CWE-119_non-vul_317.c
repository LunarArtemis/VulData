static int tcm_loop_queuecommand(
	struct Scsi_Host *sh,
	struct scsi_cmnd *sc)
{
	struct se_cmd *se_cmd;
	struct se_portal_group *se_tpg;
	struct tcm_loop_hba *tl_hba;
	struct tcm_loop_tpg *tl_tpg;

	TL_CDB_DEBUG("tcm_loop_queuecommand() %d:%d:%d:%d got CDB: 0x%02x"
		" scsi_buf_len: %u\n", sc->device->host->host_no,
		sc->device->id, sc->device->channel, sc->device->lun,
		sc->cmnd[0], scsi_bufflen(sc));
	/*
	 * Locate the tcm_loop_hba_t pointer
	 */
	tl_hba = *(struct tcm_loop_hba **)shost_priv(sc->device->host);
	tl_tpg = &tl_hba->tl_hba_tpgs[sc->device->id];
	se_tpg = &tl_tpg->tl_se_tpg;
	/*
	 * Determine the SAM Task Attribute and allocate tl_cmd and
	 * tl_cmd->tl_se_cmd from TCM infrastructure
	 */
	se_cmd = tcm_loop_allocate_core_cmd(tl_hba, se_tpg, sc);
	if (!se_cmd) {
		sc->scsi_done(sc);
		return 0;
	}
	/*
	 * Queue up the newly allocated to be processed in TCM thread context.
	*/
	transport_generic_handle_cdb_map(se_cmd);
	return 0;
}