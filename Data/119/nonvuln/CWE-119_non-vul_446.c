int
nvmet_fc_register_targetport(struct nvmet_fc_port_info *pinfo,
			struct nvmet_fc_target_template *template,
			struct device *dev,
			struct nvmet_fc_target_port **portptr)
{
	struct nvmet_fc_tgtport *newrec;
	unsigned long flags;
	int ret, idx;

	if (!template->xmt_ls_rsp || !template->fcp_op ||
	    !template->fcp_abort ||
	    !template->fcp_req_release || !template->targetport_delete ||
	    !template->max_hw_queues || !template->max_sgl_segments ||
	    !template->max_dif_sgl_segments || !template->dma_boundary) {
		ret = -EINVAL;
		goto out_regtgt_failed;
	}

	newrec = kzalloc((sizeof(*newrec) + template->target_priv_sz),
			 GFP_KERNEL);
	if (!newrec) {
		ret = -ENOMEM;
		goto out_regtgt_failed;
	}

	idx = ida_simple_get(&nvmet_fc_tgtport_cnt, 0, 0, GFP_KERNEL);
	if (idx < 0) {
		ret = -ENOSPC;
		goto out_fail_kfree;
	}

	if (!get_device(dev) && dev) {
		ret = -ENODEV;
		goto out_ida_put;
	}

	newrec->fc_target_port.node_name = pinfo->node_name;
	newrec->fc_target_port.port_name = pinfo->port_name;
	newrec->fc_target_port.private = &newrec[1];
	newrec->fc_target_port.port_id = pinfo->port_id;
	newrec->fc_target_port.port_num = idx;
	INIT_LIST_HEAD(&newrec->tgt_list);
	newrec->dev = dev;
	newrec->ops = template;
	spin_lock_init(&newrec->lock);
	INIT_LIST_HEAD(&newrec->ls_list);
	INIT_LIST_HEAD(&newrec->ls_busylist);
	INIT_LIST_HEAD(&newrec->assoc_list);
	kref_init(&newrec->ref);
	ida_init(&newrec->assoc_cnt);
	newrec->max_sg_cnt = min_t(u32, NVMET_FC_MAX_XFR_SGENTS,
					template->max_sgl_segments);

	ret = nvmet_fc_alloc_ls_iodlist(newrec);
	if (ret) {
		ret = -ENOMEM;
		goto out_free_newrec;
	}

	spin_lock_irqsave(&nvmet_fc_tgtlock, flags);
	list_add_tail(&newrec->tgt_list, &nvmet_fc_target_list);
	spin_unlock_irqrestore(&nvmet_fc_tgtlock, flags);

	*portptr = &newrec->fc_target_port;
	return 0;

out_free_newrec:
	put_device(dev);
out_ida_put:
	ida_simple_remove(&nvmet_fc_tgtport_cnt, idx);
out_fail_kfree:
	kfree(newrec);
out_regtgt_failed:
	*portptr = NULL;
	return ret;
}