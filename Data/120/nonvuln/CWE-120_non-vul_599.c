static int
ctnetlink_glue_parse(const struct nlattr *attr, struct nf_conn *ct)
{
	struct nlattr *cda[CTA_MAX+1];
	int ret;

	ret = nla_parse_nested_deprecated(cda, CTA_MAX, attr, ct_nla_policy,
					  NULL);
	if (ret < 0)
		return ret;

	return ctnetlink_glue_parse_ct((const struct nlattr **)cda, ct);
}