static int netlbl_cipsov4_listall(struct sk_buff *skb,
				  struct netlink_callback *cb)
{
	struct netlbl_cipsov4_doiwalk_arg cb_arg;
	int doi_skip = cb->args[0];

	cb_arg.nl_cb = cb;
	cb_arg.skb = skb;
	cb_arg.seq = cb->nlh->nlmsg_seq;

	cipso_v4_doi_walk(&doi_skip, netlbl_cipsov4_listall_cb, &cb_arg);

	cb->args[0] = doi_skip;
	return skb->len;
}