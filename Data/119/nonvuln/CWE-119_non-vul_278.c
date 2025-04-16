static int tcm_loop_queue_data_in(struct se_cmd *se_cmd)
{
	struct tcm_loop_cmd *tl_cmd = container_of(se_cmd,
				struct tcm_loop_cmd, tl_se_cmd);
	struct scsi_cmnd *sc = tl_cmd->sc;

	TL_CDB_DEBUG("tcm_loop_queue_data_in() called for scsi_cmnd: %p"
		     " cdb: 0x%02x\n", sc, sc->cmnd[0]);

	sc->result = SAM_STAT_GOOD;
	set_host_byte(sc, DID_OK);
	sc->scsi_done(sc);
	return 0;
}