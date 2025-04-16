static u32 tcm_loop_get_pr_transport_id_len(
	struct se_portal_group *se_tpg,
	struct se_node_acl *se_nacl,
	struct t10_pr_registration *pr_reg,
	int *format_code)
{
	struct tcm_loop_tpg *tl_tpg =
			(struct tcm_loop_tpg *)se_tpg->se_tpg_fabric_ptr;
	struct tcm_loop_hba *tl_hba = tl_tpg->tl_hba;

	switch (tl_hba->tl_proto_id) {
	case SCSI_PROTOCOL_SAS:
		return sas_get_pr_transport_id_len(se_tpg, se_nacl, pr_reg,
					format_code);
	case SCSI_PROTOCOL_FCP:
		return fc_get_pr_transport_id_len(se_tpg, se_nacl, pr_reg,
					format_code);
	case SCSI_PROTOCOL_ISCSI:
		return iscsi_get_pr_transport_id_len(se_tpg, se_nacl, pr_reg,
					format_code);
	default:
		printk(KERN_ERR "Unknown tl_proto_id: 0x%02x, using"
			" SAS emulation\n", tl_hba->tl_proto_id);
		break;
	}

	return sas_get_pr_transport_id_len(se_tpg, se_nacl, pr_reg,
			format_code);
}