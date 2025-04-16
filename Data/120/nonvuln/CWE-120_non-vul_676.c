static int ctnetlink_change_timeout(struct nf_conn *ct,
				    const struct nlattr * const cda[])
{
	u64 timeout = (u64)ntohl(nla_get_be32(cda[CTA_TIMEOUT])) * HZ;

	if (timeout > INT_MAX)
		timeout = INT_MAX;
	ct->timeout = nfct_time_stamp + (u32)timeout;

	if (test_bit(IPS_DYING_BIT, &ct->status))
		return -ETIME;

	return 0;
}