static void
nvmet_fc_remove_port(struct nvmet_port *port)
{
	struct nvmet_fc_tgtport *tgtport = port->priv;
	unsigned long flags;
	bool matched = false;

	spin_lock_irqsave(&nvmet_fc_tgtlock, flags);
	if (tgtport->port == port) {
		matched = true;
		tgtport->port = NULL;
	}
	spin_unlock_irqrestore(&nvmet_fc_tgtlock, flags);

	if (matched)
		nvmet_fc_tgtport_put(tgtport);
}