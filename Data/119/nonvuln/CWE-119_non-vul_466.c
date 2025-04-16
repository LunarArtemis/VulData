static void
nvmet_fc_handle_ls_rqst(struct nvmet_fc_tgtport *tgtport,
			struct nvmet_fc_ls_iod *iod)
{
	struct fcnvme_ls_rqst_w0 *w0 =
			(struct fcnvme_ls_rqst_w0 *)iod->rqstbuf;

	iod->lsreq->nvmet_fc_private = iod;
	iod->lsreq->rspbuf = iod->rspbuf;
	iod->lsreq->rspdma = iod->rspdma;
	iod->lsreq->done = nvmet_fc_xmt_ls_rsp_done;
	/* Be preventative. handlers will later set to valid length */
	iod->lsreq->rsplen = 0;

	iod->assoc = NULL;

	/*
	 * handlers:
	 *   parse request input, execute the request, and format the
	 *   LS response
	 */
	switch (w0->ls_cmd) {
	case FCNVME_LS_CREATE_ASSOCIATION:
		/* Creates Association and initial Admin Queue/Connection */
		nvmet_fc_ls_create_association(tgtport, iod);
		break;
	case FCNVME_LS_CREATE_CONNECTION:
		/* Creates an IO Queue/Connection */
		nvmet_fc_ls_create_connection(tgtport, iod);
		break;
	case FCNVME_LS_DISCONNECT:
		/* Terminate a Queue/Connection or the Association */
		nvmet_fc_ls_disconnect(tgtport, iod);
		break;
	default:
		iod->lsreq->rsplen = nvmet_fc_format_rjt(iod->rspbuf,
				NVME_FC_MAX_LS_BUFFER_SIZE, w0->ls_cmd,
				FCNVME_RJT_RC_INVAL, FCNVME_RJT_EXP_NONE, 0);
	}

	nvmet_fc_xmt_ls_rsp(tgtport, iod);
}