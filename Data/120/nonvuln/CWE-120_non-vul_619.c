static int
ctnetlink_dump_dying(struct sk_buff *skb, struct netlink_callback *cb)
{
	return ctnetlink_dump_list(skb, cb, true);
}