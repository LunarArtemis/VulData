static void tcm_loop_release_cmd(struct se_cmd *se_cmd)
{
	struct tcm_loop_cmd *tl_cmd = container_of(se_cmd,
				struct tcm_loop_cmd, tl_se_cmd);

	kmem_cache_free(tcm_loop_cmd_cache, tl_cmd);
}