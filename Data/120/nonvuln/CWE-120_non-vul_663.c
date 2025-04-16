static int
ctnetlink_glue_build(struct sk_buff *skb, struct nf_conn *ct,
		     enum ip_conntrack_info ctinfo,
		     u_int16_t ct_attr, u_int16_t ct_info_attr)
{
	struct nlattr *nest_parms;

	nest_parms = nla_nest_start(skb, ct_attr);
	if (!nest_parms)
		goto nla_put_failure;

	if (__ctnetlink_glue_build(skb, ct) < 0)
		goto nla_put_failure;

	nla_nest_end(skb, nest_parms);

	if (nla_put_be32(skb, ct_info_attr, htonl(ctinfo)))
		goto nla_put_failure;

	return 0;

nla_put_failure:
	return -ENOSPC;
}