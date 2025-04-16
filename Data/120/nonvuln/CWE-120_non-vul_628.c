static int
ctnetlink_change_expect(struct nf_conntrack_expect *x,
			const struct nlattr * const cda[])
{
	if (cda[CTA_EXPECT_TIMEOUT]) {
		if (!del_timer(&x->timeout))
			return -ETIME;

		x->timeout.expires = jiffies +
			ntohl(nla_get_be32(cda[CTA_EXPECT_TIMEOUT])) * HZ;
		add_timer(&x->timeout);
	}
	return 0;
}