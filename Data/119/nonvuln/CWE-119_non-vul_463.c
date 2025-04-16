static void
nvmet_fc_ls_create_association(struct nvmet_fc_tgtport *tgtport,
			struct nvmet_fc_ls_iod *iod)
{
	struct fcnvme_ls_cr_assoc_rqst *rqst =
				(struct fcnvme_ls_cr_assoc_rqst *)iod->rqstbuf;
	struct fcnvme_ls_cr_assoc_acc *acc =
				(struct fcnvme_ls_cr_assoc_acc *)iod->rspbuf;
	struct nvmet_fc_tgt_queue *queue;
	int ret = 0;

	memset(acc, 0, sizeof(*acc));

	/*
	 * FC-NVME spec changes. There are initiators sending different
	 * lengths as padding sizes for Create Association Cmd descriptor
	 * was incorrect.
	 * Accept anything of "minimum" length. Assume format per 1.15
	 * spec (with HOSTID reduced to 16 bytes), ignore how long the
	 * trailing pad length is.
	 */
	if (iod->rqstdatalen < FCNVME_LSDESC_CRA_RQST_MINLEN)
		ret = VERR_CR_ASSOC_LEN;
	else if (be32_to_cpu(rqst->desc_list_len) <
			FCNVME_LSDESC_CRA_RQST_MIN_LISTLEN)
		ret = VERR_CR_ASSOC_RQST_LEN;
	else if (rqst->assoc_cmd.desc_tag !=
			cpu_to_be32(FCNVME_LSDESC_CREATE_ASSOC_CMD))
		ret = VERR_CR_ASSOC_CMD;
	else if (be32_to_cpu(rqst->assoc_cmd.desc_len) <
			FCNVME_LSDESC_CRA_CMD_DESC_MIN_DESCLEN)
		ret = VERR_CR_ASSOC_CMD_LEN;
	else if (!rqst->assoc_cmd.ersp_ratio ||
		 (be16_to_cpu(rqst->assoc_cmd.ersp_ratio) >=
				be16_to_cpu(rqst->assoc_cmd.sqsize)))
		ret = VERR_ERSP_RATIO;

	else {
		/* new association w/ admin queue */
		iod->assoc = nvmet_fc_alloc_target_assoc(tgtport);
		if (!iod->assoc)
			ret = VERR_ASSOC_ALLOC_FAIL;
		else {
			queue = nvmet_fc_alloc_target_queue(iod->assoc, 0,
					be16_to_cpu(rqst->assoc_cmd.sqsize));
			if (!queue)
				ret = VERR_QUEUE_ALLOC_FAIL;
		}
	}

	if (ret) {
		dev_err(tgtport->dev,
			"Create Association LS failed: %s\n",
			validation_errors[ret]);
		iod->lsreq->rsplen = nvmet_fc_format_rjt(acc,
				NVME_FC_MAX_LS_BUFFER_SIZE, rqst->w0.ls_cmd,
				FCNVME_RJT_RC_LOGIC,
				FCNVME_RJT_EXP_NONE, 0);
		return;
	}

	queue->ersp_ratio = be16_to_cpu(rqst->assoc_cmd.ersp_ratio);
	atomic_set(&queue->connected, 1);
	queue->sqhd = 0;	/* best place to init value */

	/* format a response */

	iod->lsreq->rsplen = sizeof(*acc);

	nvmet_fc_format_rsp_hdr(acc, FCNVME_LS_ACC,
			fcnvme_lsdesc_len(
				sizeof(struct fcnvme_ls_cr_assoc_acc)),
			FCNVME_LS_CREATE_ASSOCIATION);
	acc->associd.desc_tag = cpu_to_be32(FCNVME_LSDESC_ASSOC_ID);
	acc->associd.desc_len =
			fcnvme_lsdesc_len(
				sizeof(struct fcnvme_lsdesc_assoc_id));
	acc->associd.association_id =
			cpu_to_be64(nvmet_fc_makeconnid(iod->assoc, 0));
	acc->connectid.desc_tag = cpu_to_be32(FCNVME_LSDESC_CONN_ID);
	acc->connectid.desc_len =
			fcnvme_lsdesc_len(
				sizeof(struct fcnvme_lsdesc_conn_id));
	acc->connectid.connection_id = acc->associd.association_id;
}