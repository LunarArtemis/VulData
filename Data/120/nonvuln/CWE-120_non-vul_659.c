static int
ctnetlink_parse_tuple(const struct nlattr * const cda[],
		      struct nf_conntrack_tuple *tuple, u32 type,
		      u_int8_t l3num, struct nf_conntrack_zone *zone)
{
	return ctnetlink_parse_tuple_filter(cda, tuple, type, l3num, zone,
					    CTA_FILTER_FLAG(ALL));
}