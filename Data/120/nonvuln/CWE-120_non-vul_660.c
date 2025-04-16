static int
ctnetlink_parse_tuple_zone(struct nlattr *attr, enum ctattr_type type,
			   struct nf_conntrack_zone *zone)
{
	int ret;

	if (zone->id != NF_CT_DEFAULT_ZONE_ID)
		return -EINVAL;

	ret = ctnetlink_parse_zone(attr, zone);
	if (ret < 0)
		return ret;

	if (type == CTA_TUPLE_REPLY)
		zone->dir = NF_CT_ZONE_DIR_REPL;
	else
		zone->dir = NF_CT_ZONE_DIR_ORIG;

	return 0;
}