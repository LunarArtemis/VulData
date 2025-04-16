static char *tcm_loop_get_endpoint_wwn(struct se_portal_group *se_tpg)
{
	struct tcm_loop_tpg *tl_tpg =
		(struct tcm_loop_tpg *)se_tpg->se_tpg_fabric_ptr;
	/*
	 * Return the passed NAA identifier for the SAS Target Port
	 */
	return &tl_tpg->tl_hba->tl_wwn_address[0];
}