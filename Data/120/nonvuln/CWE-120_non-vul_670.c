static void __exit ctnetlink_exit(void)
{
	unregister_pernet_subsys(&ctnetlink_net_ops);
	nfnetlink_subsys_unregister(&ctnl_exp_subsys);
	nfnetlink_subsys_unregister(&ctnl_subsys);
#ifdef CONFIG_NETFILTER_NETLINK_GLUE_CT
	RCU_INIT_POINTER(nfnl_ct_hook, NULL);
#endif
	synchronize_rcu();
}