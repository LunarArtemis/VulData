static void tcm_loop_tpg_release_fabric_acl(
	struct se_portal_group *se_tpg,
	struct se_node_acl *se_nacl)
{
	struct tcm_loop_nacl *tl_nacl = container_of(se_nacl,
				struct tcm_loop_nacl, se_node_acl);

	kfree(tl_nacl);
}