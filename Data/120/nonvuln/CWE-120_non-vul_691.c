static int
ctnetlink_parse_nat_setup(struct nf_conn *ct,
			  enum nf_nat_manip_type manip,
			  const struct nlattr *attr)
	__must_hold(RCU)
{
	struct nf_nat_hook *nat_hook;
	int err;

	nat_hook = rcu_dereference(nf_nat_hook);
	if (!nat_hook) {
#ifdef CONFIG_MODULES
		rcu_read_unlock();
		nfnl_unlock(NFNL_SUBSYS_CTNETLINK);
		if (request_module("nf-nat") < 0) {
			nfnl_lock(NFNL_SUBSYS_CTNETLINK);
			rcu_read_lock();
			return -EOPNOTSUPP;
		}
		nfnl_lock(NFNL_SUBSYS_CTNETLINK);
		rcu_read_lock();
		nat_hook = rcu_dereference(nf_nat_hook);
		if (nat_hook)
			return -EAGAIN;
#endif
		return -EOPNOTSUPP;
	}

	err = nat_hook->parse_nat_setup(ct, manip, attr);
	if (err == -EAGAIN) {
#ifdef CONFIG_MODULES
		rcu_read_unlock();
		nfnl_unlock(NFNL_SUBSYS_CTNETLINK);
		if (request_module("nf-nat-%u", nf_ct_l3num(ct)) < 0) {
			nfnl_lock(NFNL_SUBSYS_CTNETLINK);
			rcu_read_lock();
			return -EOPNOTSUPP;
		}
		nfnl_lock(NFNL_SUBSYS_CTNETLINK);
		rcu_read_lock();
#else
		err = -EOPNOTSUPP;
#endif
	}
	return err;
}