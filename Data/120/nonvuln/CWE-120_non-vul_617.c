static bool expect_iter_name(struct nf_conntrack_expect *exp, void *data)
{
	const struct nf_conn_help *m_help;
	const char *name = data;

	m_help = nfct_help(exp->master);

	return strcmp(m_help->helper->name, name) == 0;
}