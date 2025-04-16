static int ctnetlink_new_conntrack(struct net *net, struct sock *ctnl,
				   struct sk_buff *skb,
				   const struct nlmsghdr *nlh,
				   const struct nlattr * const cda[],
				   struct netlink_ext_ack *extack)
{
	struct nf_conntrack_tuple otuple, rtuple;
	struct nf_conntrack_tuple_hash *h = NULL;
	struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	struct nf_conn *ct;
	u_int8_t u3 = nfmsg->nfgen_family;
	struct nf_conntrack_zone zone;
	int err;

	err = ctnetlink_parse_zone(cda[CTA_ZONE], &zone);
	if (err < 0)
		return err;

	if (cda[CTA_TUPLE_ORIG]) {
		err = ctnetlink_parse_tuple(cda, &otuple, CTA_TUPLE_ORIG,
					    u3, &zone);
		if (err < 0)
			return err;
	}

	if (cda[CTA_TUPLE_REPLY]) {
		err = ctnetlink_parse_tuple(cda, &rtuple, CTA_TUPLE_REPLY,
					    u3, &zone);
		if (err < 0)
			return err;
	}

	if (cda[CTA_TUPLE_ORIG])
		h = nf_conntrack_find_get(net, &zone, &otuple);
	else if (cda[CTA_TUPLE_REPLY])
		h = nf_conntrack_find_get(net, &zone, &rtuple);

	if (h == NULL) {
		err = -ENOENT;
		if (nlh->nlmsg_flags & NLM_F_CREATE) {
			enum ip_conntrack_events events;

			if (!cda[CTA_TUPLE_ORIG] || !cda[CTA_TUPLE_REPLY])
				return -EINVAL;
			if (otuple.dst.protonum != rtuple.dst.protonum)
				return -EINVAL;

			ct = ctnetlink_create_conntrack(net, &zone, cda, &otuple,
							&rtuple, u3);
			if (IS_ERR(ct))
				return PTR_ERR(ct);

			err = 0;
			if (test_bit(IPS_EXPECTED_BIT, &ct->status))
				events = 1 << IPCT_RELATED;
			else
				events = 1 << IPCT_NEW;

			if (cda[CTA_LABELS] &&
			    ctnetlink_attach_labels(ct, cda) == 0)
				events |= (1 << IPCT_LABEL);

			nf_conntrack_eventmask_report((1 << IPCT_REPLY) |
						      (1 << IPCT_ASSURED) |
						      (1 << IPCT_HELPER) |
						      (1 << IPCT_PROTOINFO) |
						      (1 << IPCT_SEQADJ) |
						      (1 << IPCT_MARK) |
						      (1 << IPCT_SYNPROXY) |
						      events,
						      ct, NETLINK_CB(skb).portid,
						      nlmsg_report(nlh));
			nf_ct_put(ct);
		}

		return err;
	}
	/* implicit 'else' */

	err = -EEXIST;
	ct = nf_ct_tuplehash_to_ctrack(h);
	if (!(nlh->nlmsg_flags & NLM_F_EXCL)) {
		err = ctnetlink_change_conntrack(ct, cda);
		if (err == 0) {
			nf_conntrack_eventmask_report((1 << IPCT_REPLY) |
						      (1 << IPCT_ASSURED) |
						      (1 << IPCT_HELPER) |
						      (1 << IPCT_LABEL) |
						      (1 << IPCT_PROTOINFO) |
						      (1 << IPCT_SEQADJ) |
						      (1 << IPCT_MARK) |
						      (1 << IPCT_SYNPROXY),
						      ct, NETLINK_CB(skb).portid,
						      nlmsg_report(nlh));
		}
	}

	nf_ct_put(ct);
	return err;
}