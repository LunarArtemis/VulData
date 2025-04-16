static int ctnetlink_dump_helpinfo(struct sk_buff *skb,
				   const struct nf_conn *ct)
{
	struct nlattr *nest_helper;
	const struct nf_conn_help *help = nfct_help(ct);
	struct nf_conntrack_helper *helper;

	if (!help)
		return 0;

	helper = rcu_dereference(help->helper);
	if (!helper)
		goto out;

	nest_helper = nla_nest_start(skb, CTA_HELP);
	if (!nest_helper)
		goto nla_put_failure;
	if (nla_put_string(skb, CTA_HELP_NAME, helper->name))
		goto nla_put_failure;

	if (helper->to_nlattr)
		helper->to_nlattr(skb, ct);

	nla_nest_end(skb, nest_helper);
out:
	return 0;

nla_put_failure:
	return -1;
}