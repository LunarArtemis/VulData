static void ctnetlink_change_mark(struct nf_conn *ct,
				    const struct nlattr * const cda[])
{
	u32 mark, newmark, mask = 0;

	if (cda[CTA_MARK_MASK])
		mask = ~ntohl(nla_get_be32(cda[CTA_MARK_MASK]));

	mark = ntohl(nla_get_be32(cda[CTA_MARK]));
	newmark = (ct->mark & mask) ^ mark;
	if (newmark != ct->mark)
		ct->mark = newmark;
}