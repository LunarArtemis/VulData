static int __ctnetlink_glue_build(struct sk_buff *skb, struct nf_conn *ct)
{
	const struct nf_conntrack_zone *zone;
	struct nlattr *nest_parms;

	zone = nf_ct_zone(ct);

	nest_parms = nla_nest_start(skb, CTA_TUPLE_ORIG);
	if (!nest_parms)
		goto nla_put_failure;
	if (ctnetlink_dump_tuples(skb, nf_ct_tuple(ct, IP_CT_DIR_ORIGINAL)) < 0)
		goto nla_put_failure;
	if (ctnetlink_dump_zone_id(skb, CTA_TUPLE_ZONE, zone,
				   NF_CT_ZONE_DIR_ORIG) < 0)
		goto nla_put_failure;
	nla_nest_end(skb, nest_parms);

	nest_parms = nla_nest_start(skb, CTA_TUPLE_REPLY);
	if (!nest_parms)
		goto nla_put_failure;
	if (ctnetlink_dump_tuples(skb, nf_ct_tuple(ct, IP_CT_DIR_REPLY)) < 0)
		goto nla_put_failure;
	if (ctnetlink_dump_zone_id(skb, CTA_TUPLE_ZONE, zone,
				   NF_CT_ZONE_DIR_REPL) < 0)
		goto nla_put_failure;
	nla_nest_end(skb, nest_parms);

	if (ctnetlink_dump_zone_id(skb, CTA_ZONE, zone,
				   NF_CT_DEFAULT_ZONE_DIR) < 0)
		goto nla_put_failure;

	if (ctnetlink_dump_id(skb, ct) < 0)
		goto nla_put_failure;

	if (ctnetlink_dump_status(skb, ct) < 0)
		goto nla_put_failure;

	if (ctnetlink_dump_timeout(skb, ct) < 0)
		goto nla_put_failure;

	if (ctnetlink_dump_protoinfo(skb, ct) < 0)
		goto nla_put_failure;

	if (ctnetlink_dump_helpinfo(skb, ct) < 0)
		goto nla_put_failure;

#ifdef CONFIG_NF_CONNTRACK_SECMARK
	if (ct->secmark && ctnetlink_dump_secctx(skb, ct) < 0)
		goto nla_put_failure;
#endif
	if (ct->master && ctnetlink_dump_master(skb, ct) < 0)
		goto nla_put_failure;

	if ((ct->status & IPS_SEQ_ADJUST) &&
	    ctnetlink_dump_ct_seq_adj(skb, ct) < 0)
		goto nla_put_failure;

	if (ctnetlink_dump_ct_synproxy(skb, ct) < 0)
		goto nla_put_failure;

#ifdef CONFIG_NF_CONNTRACK_MARK
	if (ct->mark && ctnetlink_dump_mark(skb, ct) < 0)
		goto nla_put_failure;
#endif
	if (ctnetlink_dump_labels(skb, ct) < 0)
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -ENOSPC;
}