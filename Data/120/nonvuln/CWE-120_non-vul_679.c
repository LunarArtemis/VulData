static int ctnetlink_stat_ct(struct net *net, struct sock *ctnl,
			     struct sk_buff *skb, const struct nlmsghdr *nlh,
			     const struct nlattr * const cda[],
			     struct netlink_ext_ack *extack)
{
	struct sk_buff *skb2;
	int err;

	skb2 = nlmsg_new(NLMSG_DEFAULT_SIZE, GFP_KERNEL);
	if (skb2 == NULL)
		return -ENOMEM;

	err = ctnetlink_stat_ct_fill_info(skb2, NETLINK_CB(skb).portid,
					  nlh->nlmsg_seq,
					  NFNL_MSG_TYPE(nlh->nlmsg_type),
					  sock_net(skb->sk));
	if (err <= 0)
		goto free;

	err = netlink_unicast(ctnl, skb2, NETLINK_CB(skb).portid, MSG_DONTWAIT);
	if (err < 0)
		goto out;

	return 0;

free:
	kfree_skb(skb2);
out:
	/* this avoids a loop in nfnetlink. */
	return err == -EAGAIN ? -ENOBUFS : err;
}