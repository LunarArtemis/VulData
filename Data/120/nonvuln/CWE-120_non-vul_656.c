static int ctnetlink_change_protoinfo(struct nf_conn *ct,
				      const struct nlattr * const cda[])
{
	const struct nlattr *attr = cda[CTA_PROTOINFO];
	const struct nf_conntrack_l4proto *l4proto;
	struct nlattr *tb[CTA_PROTOINFO_MAX+1];
	int err = 0;

	err = nla_parse_nested_deprecated(tb, CTA_PROTOINFO_MAX, attr,
					  protoinfo_policy, NULL);
	if (err < 0)
		return err;

	l4proto = nf_ct_l4proto_find(nf_ct_protonum(ct));
	if (l4proto->from_nlattr)
		err = l4proto->from_nlattr(tb, ct);

	return err;
}