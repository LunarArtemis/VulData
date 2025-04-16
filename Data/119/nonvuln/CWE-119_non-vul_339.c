static int tcm_loop_sess_logged_in(struct se_session *se_sess)
{
	/*
	 * Assume that TL Nexus is always active
	 */
	return 1;
}