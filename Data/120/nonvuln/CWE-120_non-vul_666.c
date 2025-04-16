static int ctnetlink_dump_ct_synproxy(struct sk_buff *skb, struct nf_conn *ct)
{
	struct nf_conn_synproxy *synproxy = nfct_synproxy(ct);
	struct nlattr *nest_parms;

	if (!synproxy)
		return 0;

	nest_parms = nla_nest_start(skb, CTA_SYNPROXY);
	if (!nest_parms)
		goto nla_put_failure;

	if (nla_put_be32(skb, CTA_SYNPROXY_ISN, htonl(synproxy->isn)) ||
	    nla_put_be32(skb, CTA_SYNPROXY_ITS, htonl(synproxy->its)) ||
	    nla_put_be32(skb, CTA_SYNPROXY_TSOFF, htonl(synproxy->tsoff)))
		goto nla_put_failure;

	nla_nest_end(skb, nest_parms);

	return 0;

nla_put_failure:
	return -1;
}