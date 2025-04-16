static size_t ctnetlink_proto_size(const struct nf_conn *ct)
{
	const struct nf_conntrack_l4proto *l4proto;
	size_t len, len4 = 0;

	len = nla_policy_len(cta_ip_nla_policy, CTA_IP_MAX + 1);
	len *= 3u; /* ORIG, REPLY, MASTER */

	l4proto = nf_ct_l4proto_find(nf_ct_protonum(ct));
	len += l4proto->nlattr_size;
	if (l4proto->nlattr_tuple_size) {
		len4 = l4proto->nlattr_tuple_size();
		len4 *= 3u; /* ORIG, REPLY, MASTER */
	}

	return len + len4;
}