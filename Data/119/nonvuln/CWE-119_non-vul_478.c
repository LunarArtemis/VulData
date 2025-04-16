int
nvmet_fc_rcv_ls_req(struct nvmet_fc_target_port *target_port,
			struct nvmefc_tgt_ls_req *lsreq,
			void *lsreqbuf, u32 lsreqbuf_len)
{
	struct nvmet_fc_tgtport *tgtport = targetport_to_tgtport(target_port);
	struct nvmet_fc_ls_iod *iod;

	if (lsreqbuf_len > NVME_FC_MAX_LS_BUFFER_SIZE)
		return -E2BIG;

	if (!nvmet_fc_tgtport_get(tgtport))
		return -ESHUTDOWN;

	iod = nvmet_fc_alloc_ls_iod(tgtport);
	if (!iod) {
		nvmet_fc_tgtport_put(tgtport);
		return -ENOENT;
	}

	iod->lsreq = lsreq;
	iod->fcpreq = NULL;
	memcpy(iod->rqstbuf, lsreqbuf, lsreqbuf_len);
	iod->rqstdatalen = lsreqbuf_len;

	schedule_work(&iod->work);

	return 0;
}