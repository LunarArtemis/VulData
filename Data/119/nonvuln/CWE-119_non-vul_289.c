static int tcm_loop_driver_remove(struct device *dev)
{
	struct tcm_loop_hba *tl_hba;
	struct Scsi_Host *sh;

	tl_hba = to_tcm_loop_hba(dev);
	sh = tl_hba->sh;

	scsi_remove_host(sh);
	scsi_host_put(sh);
	return 0;
}