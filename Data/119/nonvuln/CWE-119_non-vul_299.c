static u16 tcm_loop_get_tag(struct se_portal_group *se_tpg)
{
	struct tcm_loop_tpg *tl_tpg =
		(struct tcm_loop_tpg *)se_tpg->se_tpg_fabric_ptr;
	/*
	 * This Tag is used when forming SCSI Name identifier in EVPD=1 0x83
	 * to represent the SCSI Target Port.
	 */
	return tl_tpg->tl_tpgt;
}