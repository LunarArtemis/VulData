static int ctnetlink_del_conntrack(struct net *net, struct sock *ctnl,
				   struct sk_buff *skb,
				   const struct nlmsghdr *nlh,
				   const struct nlattr * const cda[],
				   struct netlink_ext_ack *extack)
{
	struct nf_conntrack_tuple_hash *h;
	struct nf_conntrack_tuple tuple;
	struct nf_conn *ct;
	struct nfgenmsg *nfmsg = nlmsg_data(nlh);
	struct nf_conntrack_zone zone;
	int err;

	err = ctnetlink_parse_zone(cda[CTA_ZONE], &zone);
	if (err < 0)
		return err;

	if (cda[CTA_TUPLE_ORIG])
		err = ctnetlink_parse_tuple(cda, &tuple, CTA_TUPLE_ORIG,
					    nfmsg->nfgen_family, &zone);
	else if (cda[CTA_TUPLE_REPLY])
		err = ctnetlink_parse_tuple(cda, &tuple, CTA_TUPLE_REPLY,
					    nfmsg->nfgen_family, &zone);
	else {
		u_int8_t u3 = nfmsg->version ? nfmsg->nfgen_family : AF_UNSPEC;

		return ctnetlink_flush_conntrack(net, cda,
						 NETLINK_CB(skb).portid,
						 nlmsg_report(nlh), u3);
	}

	if (err < 0)
		return err;

	h = nf_conntrack_find_get(net, &zone, &tuple);
	if (!h)
		return -ENOENT;

	ct = nf_ct_tuplehash_to_ctrack(h);

	if (test_bit(IPS_OFFLOAD_BIT, &ct->status)) {
		nf_ct_put(ct);
		return -EBUSY;
	}

	if (cda[CTA_ID]) {
		__be32 id = nla_get_be32(cda[CTA_ID]);

		if (id != (__force __be32)nf_ct_get_id(ct)) {
			nf_ct_put(ct);
			return -ENOENT;
		}
	}

	nf_ct_delete(ct, NETLINK_CB(skb).portid, nlmsg_report(nlh));
	nf_ct_put(ct);

	return 0;
}