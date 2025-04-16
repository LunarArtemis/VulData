static int ctnetlink_flush_iterate(struct nf_conn *ct, void *data)
{
	if (test_bit(IPS_OFFLOAD_BIT, &ct->status))
		return 0;

	return ctnetlink_filter_match(ct, data);
}