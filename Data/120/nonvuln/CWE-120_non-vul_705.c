static int ctnetlink_parse_tuple_proto(struct nlattr *attr,
				       struct nf_conntrack_tuple *tuple,
				       u_int32_t flags)
{
	const struct nf_conntrack_l4proto *l4proto;
	struct nlattr *tb[CTA_PROTO_MAX+1];
	int ret = 0;

	ret = nla_parse_nested_deprecated(tb, CTA_PROTO_MAX, attr,
					  proto_nla_policy, NULL);
	if (ret < 0)
		return ret;

	if (!(flags & CTA_FILTER_FLAG(CTA_PROTO_NUM)))
		return 0;

	if (!tb[CTA_PROTO_NUM])
		return -EINVAL;

	tuple->dst.protonum = nla_get_u8(tb[CTA_PROTO_NUM]);

	rcu_read_lock();
	l4proto = nf_ct_l4proto_find(tuple->dst.protonum);

	if (likely(l4proto->nlattr_to_tuple)) {
		ret = nla_validate_nested_deprecated(attr, CTA_PROTO_MAX,
						     l4proto->nla_policy,
						     NULL);
		if (ret == 0)
			ret = l4proto->nlattr_to_tuple(tb, tuple, flags);
	}

	rcu_read_unlock();

	return ret;
}