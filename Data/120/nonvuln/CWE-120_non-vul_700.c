static int ctnetlink_dump_secctx(struct sk_buff *skb, const struct nf_conn *ct)
{
	struct nlattr *nest_secctx;
	int len, ret;
	char *secctx;

	ret = security_secid_to_secctx(ct->secmark, &secctx, &len);
	if (ret)
		return 0;

	ret = -1;
	nest_secctx = nla_nest_start(skb, CTA_SECCTX);
	if (!nest_secctx)
		goto nla_put_failure;

	if (nla_put_string(skb, CTA_SECCTX_NAME, secctx))
		goto nla_put_failure;
	nla_nest_end(skb, nest_secctx);

	ret = 0;
nla_put_failure:
	security_release_secctx(secctx, len);
	return ret;
}