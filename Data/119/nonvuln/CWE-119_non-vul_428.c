static void
nvmet_fc_free_ls_iod(struct nvmet_fc_tgtport *tgtport,
			struct nvmet_fc_ls_iod *iod)
{
	unsigned long flags;

	spin_lock_irqsave(&tgtport->lock, flags);
	list_move(&iod->ls_list, &tgtport->ls_list);
	spin_unlock_irqrestore(&tgtport->lock, flags);
}