static int ctnetlink_dump_info(struct sk_buff *skb, struct nf_conn *ct)
{
	if (ctnetlink_dump_status(skb, ct) < 0 ||
	    ctnetlink_dump_mark(skb, ct) < 0 ||
	    ctnetlink_dump_secctx(skb, ct) < 0 ||
	    ctnetlink_dump_id(skb, ct) < 0 ||
	    ctnetlink_dump_use(skb, ct) < 0 ||
	    ctnetlink_dump_master(skb, ct) < 0)
		return -1;

	if (!test_bit(IPS_OFFLOAD_BIT, &ct->status) &&
	    (ctnetlink_dump_timeout(skb, ct) < 0 ||
	     ctnetlink_dump_protoinfo(skb, ct) < 0))
		return -1;

	return 0;
}