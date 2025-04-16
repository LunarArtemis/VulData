static int
ctnetlink_parse_zone(const struct nlattr *attr,
		     struct nf_conntrack_zone *zone)
{
	nf_ct_zone_init(zone, NF_CT_DEFAULT_ZONE_ID,
			NF_CT_DEFAULT_ZONE_DIR, 0);
#ifdef CONFIG_NF_CONNTRACK_ZONES
	if (attr)
		zone->id = ntohs(nla_get_be16(attr));
#else
	if (attr)
		return -EOPNOTSUPP;
#endif
	return 0;
}