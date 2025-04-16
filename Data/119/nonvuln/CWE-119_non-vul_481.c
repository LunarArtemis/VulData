static void
nvmet_fc_ls_disconnect(struct nvmet_fc_tgtport *tgtport,
			struct nvmet_fc_ls_iod *iod)
{
	struct fcnvme_ls_disconnect_rqst *rqst =
			(struct fcnvme_ls_disconnect_rqst *)iod->rqstbuf;
	struct fcnvme_ls_disconnect_acc *acc =
			(struct fcnvme_ls_disconnect_acc *)iod->rspbuf;
	struct nvmet_fc_tgt_queue *queue = NULL;
	struct nvmet_fc_tgt_assoc *assoc;
	int ret = 0;
	bool del_assoc = false;

	memset(acc, 0, sizeof(*acc));

	if (iod->rqstdatalen < sizeof(struct fcnvme_ls_disconnect_rqst))
		ret = VERR_DISCONN_LEN;
	else if (rqst->desc_list_len !=
			fcnvme_lsdesc_len(
				sizeof(struct fcnvme_ls_disconnect_rqst)))
		ret = VERR_DISCONN_RQST_LEN;
	else if (rqst->associd.desc_tag != cpu_to_be32(FCNVME_LSDESC_ASSOC_ID))
		ret = VERR_ASSOC_ID;
	else if (rqst->associd.desc_len !=
			fcnvme_lsdesc_len(
				sizeof(struct fcnvme_lsdesc_assoc_id)))
		ret = VERR_ASSOC_ID_LEN;
	else if (rqst->discon_cmd.desc_tag !=
			cpu_to_be32(FCNVME_LSDESC_DISCONN_CMD))
		ret = VERR_DISCONN_CMD;
	else if (rqst->discon_cmd.desc_len !=
			fcnvme_lsdesc_len(
				sizeof(struct fcnvme_lsdesc_disconn_cmd)))
		ret = VERR_DISCONN_CMD_LEN;
	else if ((rqst->discon_cmd.scope != FCNVME_DISCONN_ASSOCIATION) &&
			(rqst->discon_cmd.scope != FCNVME_DISCONN_CONNECTION))
		ret = VERR_DISCONN_SCOPE;
	else {
		/* match an active association */
		assoc = nvmet_fc_find_target_assoc(tgtport,
				be64_to_cpu(rqst->associd.association_id));
		iod->assoc = assoc;
		if (assoc) {
			if (rqst->discon_cmd.scope ==
					FCNVME_DISCONN_CONNECTION) {
				queue = nvmet_fc_find_target_queue(tgtport,
						be64_to_cpu(
							rqst->discon_cmd.id));
				if (!queue) {
					nvmet_fc_tgt_a_put(assoc);
					ret = VERR_NO_CONN;
				}
			}
		} else
			ret = VERR_NO_ASSOC;
	}

	if (ret) {
		dev_err(tgtport->dev,
			"Disconnect LS failed: %s\n",
			validation_errors[ret]);
		iod->lsreq->rsplen = nvmet_fc_format_rjt(acc,
				NVME_FC_MAX_LS_BUFFER_SIZE, rqst->w0.ls_cmd,
				(ret == VERR_NO_ASSOC) ?
					FCNVME_RJT_RC_INV_ASSOC :
					(ret == VERR_NO_CONN) ?
						FCNVME_RJT_RC_INV_CONN :
						FCNVME_RJT_RC_LOGIC,
				FCNVME_RJT_EXP_NONE, 0);
		return;
	}

	/* format a response */

	iod->lsreq->rsplen = sizeof(*acc);

	nvmet_fc_format_rsp_hdr(acc, FCNVME_LS_ACC,
			fcnvme_lsdesc_len(
				sizeof(struct fcnvme_ls_disconnect_acc)),
			FCNVME_LS_DISCONNECT);


	/* are we to delete a Connection ID (queue) */
	if (queue) {
		int qid = queue->qid;

		nvmet_fc_delete_target_queue(queue);

		/* release the get taken by find_target_queue */
		nvmet_fc_tgt_q_put(queue);

		/* tear association down if io queue terminated */
		if (!qid)
			del_assoc = true;
	}

	/* release get taken in nvmet_fc_find_target_assoc */
	nvmet_fc_tgt_a_put(iod->assoc);

	if (del_assoc)
		nvmet_fc_delete_target_assoc(iod->assoc);
}