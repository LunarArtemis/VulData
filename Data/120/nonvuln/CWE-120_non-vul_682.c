static int
ctnetlink_parse_expect_nat(const struct nlattr *attr,
			   struct nf_conntrack_expect *exp,
			   u_int8_t u3)
{
#if IS_ENABLED(CONFIG_NF_NAT)
	struct nlattr *tb[CTA_EXPECT_NAT_MAX+1];
	struct nf_conntrack_tuple nat_tuple = {};
	int err;

	err = nla_parse_nested_deprecated(tb, CTA_EXPECT_NAT_MAX, attr,
					  exp_nat_nla_policy, NULL);
	if (err < 0)
		return err;

	if (!tb[CTA_EXPECT_NAT_DIR] || !tb[CTA_EXPECT_NAT_TUPLE])
		return -EINVAL;

	err = ctnetlink_parse_tuple((const struct nlattr * const *)tb,
				    &nat_tuple, CTA_EXPECT_NAT_TUPLE,
				    u3, NULL);
	if (err < 0)
		return err;

	exp->saved_addr = nat_tuple.src.u3;
	exp->saved_proto = nat_tuple.src.u;
	exp->dir = ntohl(nla_get_be32(tb[CTA_EXPECT_NAT_DIR]));

	return 0;
#else
	return -EOPNOTSUPP;
#endif
}