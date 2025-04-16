static int
ctnetlink_ct_stat_cpu_fill_info(struct sk_buff *skb, u32 portid, u32 seq,
				__u16 cpu, const struct ip_conntrack_stat *st)
{
	struct nlmsghdr *nlh;
	struct nfgenmsg *nfmsg;
	unsigned int flags = portid ? NLM_F_MULTI : 0, event;

	event = nfnl_msg_type(NFNL_SUBSYS_CTNETLINK,
			      IPCTNL_MSG_CT_GET_STATS_CPU);
	nlh = nlmsg_put(skb, portid, seq, event, sizeof(*nfmsg), flags);
	if (nlh == NULL)
		goto nlmsg_failure;

	nfmsg = nlmsg_data(nlh);
	nfmsg->nfgen_family = AF_UNSPEC;
	nfmsg->version      = NFNETLINK_V0;
	nfmsg->res_id	    = htons(cpu);

	if (nla_put_be32(skb, CTA_STATS_FOUND, htonl(st->found)) ||
	    nla_put_be32(skb, CTA_STATS_INVALID, htonl(st->invalid)) ||
	    nla_put_be32(skb, CTA_STATS_IGNORE, htonl(st->ignore)) ||
	    nla_put_be32(skb, CTA_STATS_INSERT, htonl(st->insert)) ||
	    nla_put_be32(skb, CTA_STATS_INSERT_FAILED,
				htonl(st->insert_failed)) ||
	    nla_put_be32(skb, CTA_STATS_DROP, htonl(st->drop)) ||
	    nla_put_be32(skb, CTA_STATS_EARLY_DROP, htonl(st->early_drop)) ||
	    nla_put_be32(skb, CTA_STATS_ERROR, htonl(st->error)) ||
	    nla_put_be32(skb, CTA_STATS_SEARCH_RESTART,
				htonl(st->search_restart)))
		goto nla_put_failure;

	nlmsg_end(skb, nlh);
	return skb->len;

nla_put_failure:
nlmsg_failure:
	nlmsg_cancel(skb, nlh);
	return -1;
}