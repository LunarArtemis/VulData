static int
ctnetlink_setup_nat(struct nf_conn *ct, const struct nlattr * const cda[])
{
#if IS_ENABLED(CONFIG_NF_NAT)
	int ret;

	if (!cda[CTA_NAT_DST] && !cda[CTA_NAT_SRC])
		return 0;

	ret = ctnetlink_parse_nat_setup(ct, NF_NAT_MANIP_DST,
					cda[CTA_NAT_DST]);
	if (ret < 0)
		return ret;

	return ctnetlink_parse_nat_setup(ct, NF_NAT_MANIP_SRC,
					 cda[CTA_NAT_SRC]);
#else
	if (!cda[CTA_NAT_DST] && !cda[CTA_NAT_SRC])
		return 0;
	return -EOPNOTSUPP;
#endif
}