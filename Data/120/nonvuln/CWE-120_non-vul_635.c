static int ctnetlink_change_synproxy(struct nf_conn *ct,
				     const struct nlattr * const cda[])
{
	struct nf_conn_synproxy *synproxy = nfct_synproxy(ct);
	struct nlattr *tb[CTA_SYNPROXY_MAX + 1];
	int err;

	if (!synproxy)
		return 0;

	err = nla_parse_nested_deprecated(tb, CTA_SYNPROXY_MAX,
					  cda[CTA_SYNPROXY], synproxy_policy,
					  NULL);
	if (err < 0)
		return err;

	if (!tb[CTA_SYNPROXY_ISN] ||
	    !tb[CTA_SYNPROXY_ITS] ||
	    !tb[CTA_SYNPROXY_TSOFF])
		return -EINVAL;

	synproxy->isn = ntohl(nla_get_be32(tb[CTA_SYNPROXY_ISN]));
	synproxy->its = ntohl(nla_get_be32(tb[CTA_SYNPROXY_ITS]));
	synproxy->tsoff = ntohl(nla_get_be32(tb[CTA_SYNPROXY_TSOFF]));

	return 0;
}