static int __init ctnetlink_init(void)
{
	int ret;

	ret = nfnetlink_subsys_register(&ctnl_subsys);
	if (ret < 0) {
		pr_err("ctnetlink_init: cannot register with nfnetlink.\n");
		goto err_out;
	}

	ret = nfnetlink_subsys_register(&ctnl_exp_subsys);
	if (ret < 0) {
		pr_err("ctnetlink_init: cannot register exp with nfnetlink.\n");
		goto err_unreg_subsys;
	}

	ret = register_pernet_subsys(&ctnetlink_net_ops);
	if (ret < 0) {
		pr_err("ctnetlink_init: cannot register pernet operations\n");
		goto err_unreg_exp_subsys;
	}
#ifdef CONFIG_NETFILTER_NETLINK_GLUE_CT
	/* setup interaction between nf_queue and nf_conntrack_netlink. */
	RCU_INIT_POINTER(nfnl_ct_hook, &ctnetlink_glue_hook);
#endif
	return 0;

err_unreg_exp_subsys:
	nfnetlink_subsys_unregister(&ctnl_exp_subsys);
err_unreg_subsys:
	nfnetlink_subsys_unregister(&ctnl_subsys);
err_out:
	return ret;
}