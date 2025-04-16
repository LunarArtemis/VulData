static void
nvmet_fc_free_tgtport(struct kref *ref)
{
	struct nvmet_fc_tgtport *tgtport =
		container_of(ref, struct nvmet_fc_tgtport, ref);
	struct device *dev = tgtport->dev;
	unsigned long flags;

	spin_lock_irqsave(&nvmet_fc_tgtlock, flags);
	list_del(&tgtport->tgt_list);
	spin_unlock_irqrestore(&nvmet_fc_tgtlock, flags);

	nvmet_fc_free_ls_iodlist(tgtport);

	/* let the LLDD know we've finished tearing it down */
	tgtport->ops->targetport_delete(&tgtport->fc_target_port);

	ida_simple_remove(&nvmet_fc_tgtport_cnt,
			tgtport->fc_target_port.port_num);

	ida_destroy(&tgtport->assoc_cnt);

	kfree(tgtport);

	put_device(dev);
}