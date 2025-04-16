static int tcm_loop_queue_status(struct se_cmd *se_cmd)
{
	struct tcm_loop_cmd *tl_cmd = container_of(se_cmd,
				struct tcm_loop_cmd, tl_se_cmd);
	struct scsi_cmnd *sc = tl_cmd->sc;

	TL_CDB_DEBUG("tcm_loop_queue_status() called for scsi_cmnd: %p"
			" cdb: 0x%02x\n", sc, sc->cmnd[0]);

	if (se_cmd->sense_buffer &&
	   ((se_cmd->se_cmd_flags & SCF_TRANSPORT_TASK_SENSE) ||
	    (se_cmd->se_cmd_flags & SCF_EMULATED_TASK_SENSE))) {

		memcpy(sc->sense_buffer, se_cmd->sense_buffer,
				SCSI_SENSE_BUFFERSIZE);
		sc->result = SAM_STAT_CHECK_CONDITION;
		set_driver_byte(sc, DRIVER_SENSE);
	} else
		sc->result = se_cmd->scsi_status;

	set_host_byte(sc, DID_OK);
	sc->scsi_done(sc);
	return 0;
}