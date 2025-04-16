static int
nvmet_fc_add_port(struct nvmet_port *port)
{
	struct nvmet_fc_tgtport *tgtport;
	struct nvmet_fc_traddr traddr = { 0L, 0L };
	unsigned long flags;
	int ret;

	/* validate the address info */
	if ((port->disc_addr.trtype != NVMF_TRTYPE_FC) ||
	    (port->disc_addr.adrfam != NVMF_ADDR_FAMILY_FC))
		return -EINVAL;

	/* map the traddr address info to a target port */

	ret = nvme_fc_parse_traddr(&traddr, port->disc_addr.traddr,
			sizeof(port->disc_addr.traddr));
	if (ret)
		return ret;

	ret = -ENXIO;
	spin_lock_irqsave(&nvmet_fc_tgtlock, flags);
	list_for_each_entry(tgtport, &nvmet_fc_target_list, tgt_list) {
		if ((tgtport->fc_target_port.node_name == traddr.nn) &&
		    (tgtport->fc_target_port.port_name == traddr.pn)) {
			/* a FC port can only be 1 nvmet port id */
			if (!tgtport->port) {
				tgtport->port = port;
				port->priv = tgtport;
				nvmet_fc_tgtport_get(tgtport);
				ret = 0;
			} else
				ret = -EALREADY;
			break;
		}
	}
	spin_unlock_irqrestore(&nvmet_fc_tgtlock, flags);
	return ret;
}