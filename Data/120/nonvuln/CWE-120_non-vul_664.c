static int ctnetlink_change_helper(struct nf_conn *ct,
				   const struct nlattr * const cda[])
{
	struct nf_conntrack_helper *helper;
	struct nf_conn_help *help = nfct_help(ct);
	char *helpname = NULL;
	struct nlattr *helpinfo = NULL;
	int err;

	err = ctnetlink_parse_help(cda[CTA_HELP], &helpname, &helpinfo);
	if (err < 0)
		return err;

	/* don't change helper of sibling connections */
	if (ct->master) {
		/* If we try to change the helper to the same thing twice,
		 * treat the second attempt as a no-op instead of returning
		 * an error.
		 */
		err = -EBUSY;
		if (help) {
			rcu_read_lock();
			helper = rcu_dereference(help->helper);
			if (helper && !strcmp(helper->name, helpname))
				err = 0;
			rcu_read_unlock();
		}

		return err;
	}

	if (!strcmp(helpname, "")) {
		if (help && help->helper) {
			/* we had a helper before ... */
			nf_ct_remove_expectations(ct);
			RCU_INIT_POINTER(help->helper, NULL);
		}

		return 0;
	}

	rcu_read_lock();
	helper = __nf_conntrack_helper_find(helpname, nf_ct_l3num(ct),
					    nf_ct_protonum(ct));
	if (helper == NULL) {
		rcu_read_unlock();
		return -EOPNOTSUPP;
	}

	if (help) {
		if (help->helper == helper) {
			/* update private helper data if allowed. */
			if (helper->from_nlattr)
				helper->from_nlattr(helpinfo, ct);
			err = 0;
		} else
			err = -EBUSY;
	} else {
		/* we cannot set a helper for an existing conntrack */
		err = -EOPNOTSUPP;
	}

	rcu_read_unlock();
	return err;
}