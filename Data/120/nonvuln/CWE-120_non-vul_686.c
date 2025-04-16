static int ctnetlink_dump_zone_id(struct sk_buff *skb, int attrtype,
				  const struct nf_conntrack_zone *zone, int dir)
{
	if (zone->id == NF_CT_DEFAULT_ZONE_ID || zone->dir != dir)
		return 0;
	if (nla_put_be16(skb, attrtype, htons(zone->id)))
		goto nla_put_failure;
	return 0;

nla_put_failure:
	return -1;
}