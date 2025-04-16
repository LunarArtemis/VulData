static int tcm_loop_write_pending(struct se_cmd *se_cmd)
{
	/*
	 * Since Linux/SCSI has already sent down a struct scsi_cmnd
	 * sc->sc_data_direction of DMA_TO_DEVICE with struct scatterlist array
	 * memory, and memory has already been mapped to struct se_cmd->t_mem_list
	 * format with transport_generic_map_mem_to_cmd().
	 *
	 * We now tell TCM to add this WRITE CDB directly into the TCM storage
	 * object execution queue.
	 */
	transport_generic_process_write(se_cmd);
	return 0;
}