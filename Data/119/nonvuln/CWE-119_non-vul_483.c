static struct nvmet_fc_ls_iod *
nvmet_fc_alloc_ls_iod(struct nvmet_fc_tgtport *tgtport)
{
	struct nvmet_fc_ls_iod *iod;
	unsigned long flags;

	spin_lock_irqsave(&tgtport->lock, flags);
	iod = list_first_entry_or_null(&tgtport->ls_list,
					struct nvmet_fc_ls_iod, ls_list);
	if (iod)
		list_move_tail(&iod->ls_list, &tgtport->ls_busylist);
	spin_unlock_irqrestore(&tgtport->lock, flags);
	return iod;
}