static void __net_exit ctnetlink_net_exit_batch(struct list_head *net_exit_list)
{
	struct net *net;

	list_for_each_entry(net, net_exit_list, exit_list)
		ctnetlink_net_exit(net);

	/* wait for other cpus until they are done with ctnl_notifiers */
	synchronize_rcu();
}