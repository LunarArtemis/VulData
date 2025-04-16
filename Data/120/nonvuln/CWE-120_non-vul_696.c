static int
ctnetlink_fill_info(struct sk_buff *skb, u32 portid, u32 seq, u32 type,
		    struct nf_conn *ct, bool extinfo, unsigned int flags)
{
	const struct nf_conntrack_zone *zone;
	struct nlmsghdr *nlh;
	struct nfgenmsg *nfmsg;
	struct nlattr *nest_parms;
	unsigned int event;

	if (portid)
		flags |= NLM_F_MULTI;
	event = nfnl_msg_type(NFNL_SUBSYS_CTNETLINK, IPCTNL_MSG_CT_NEW);
	nlh = nlmsg_put(skb, portid, seq, event, sizeof(*nfmsg), flags);
	if (nlh == NULL)
		goto nlmsg_failure;

	nfmsg = nlmsg_data(nlh);
	nfmsg->nfgen_family = nf_ct_l3num(ct);
	nfmsg->version      = NFNETLINK_V0;
	nfmsg->res_id	    = 0;

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

	if (ctnetlink_dump_info(skb, ct) < 0)
		goto nla_put_failure;
	if (extinfo && ctnetlink_dump_extinfo(skb, ct, type) < 0)
		goto nla_put_failure;

	nlmsg_end(skb, nlh);
	return skb->len;

nlmsg_failure:
nla_put_failure:
	nlmsg_cancel(skb, nlh);
	return -1;
}