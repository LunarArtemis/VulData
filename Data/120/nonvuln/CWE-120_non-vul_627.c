static int __net_init ctnetlink_net_init(struct net *net)
{
#ifdef CONFIG_NF_CONNTRACK_EVENTS
	int ret;

	ret = nf_conntrack_register_notifier(net, &ctnl_notifier);
	if (ret < 0) {
		pr_err("ctnetlink_init: cannot register notifier.\n");
		goto err_out;
	}

	ret = nf_ct_expect_register_notifier(net, &ctnl_notifier_exp);
	if (ret < 0) {
		pr_err("ctnetlink_init: cannot expect register notifier.\n");
		goto err_unreg_notifier;
	}
#endif
	return 0;

#ifdef CONFIG_NF_CONNTRACK_EVENTS
err_unreg_notifier:
	nf_conntrack_unregister_notifier(net, &ctnl_notifier);
err_out:
	return ret;
#endif
}