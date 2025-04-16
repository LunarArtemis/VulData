static char *tcm_loop_dump_proto_id(struct tcm_loop_hba *tl_hba)
{
	switch (tl_hba->tl_proto_id) {
	case SCSI_PROTOCOL_SAS:
		return "SAS";
	case SCSI_PROTOCOL_FCP:
		return "FCP";
	case SCSI_PROTOCOL_ISCSI:
		return "iSCSI";
	default:
		break;
	}

	return "Unknown";
}