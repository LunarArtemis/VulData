static int
ctnetlink_create_expect(struct net *net,
			const struct nf_conntrack_zone *zone,
			const struct nlattr * const cda[],
			u_int8_t u3, u32 portid, int report)
{
	struct nf_conntrack_tuple tuple, mask, master_tuple;
	struct nf_conntrack_tuple_hash *h = NULL;
	struct nf_conntrack_helper *helper = NULL;
	struct nf_conntrack_expect *exp;
	struct nf_conn *ct;
	int err;

	/* caller guarantees that those three CTA_EXPECT_* exist */
	err = ctnetlink_parse_tuple(cda, &tuple, CTA_EXPECT_TUPLE,
				    u3, NULL);
	if (err < 0)
		return err;
	err = ctnetlink_parse_tuple(cda, &mask, CTA_EXPECT_MASK,
				    u3, NULL);
	if (err < 0)
		return err;
	err = ctnetlink_parse_tuple(cda, &master_tuple, CTA_EXPECT_MASTER,
				    u3, NULL);
	if (err < 0)
		return err;

	/* Look for master conntrack of this expectation */
	h = nf_conntrack_find_get(net, zone, &master_tuple);
	if (!h)
		return -ENOENT;
	ct = nf_ct_tuplehash_to_ctrack(h);

	rcu_read_lock();
	if (cda[CTA_EXPECT_HELP_NAME]) {
		const char *helpname = nla_data(cda[CTA_EXPECT_HELP_NAME]);

		helper = __nf_conntrack_helper_find(helpname, u3,
						    nf_ct_protonum(ct));
		if (helper == NULL) {
			rcu_read_unlock();
#ifdef CONFIG_MODULES
			if (request_module("nfct-helper-%s", helpname) < 0) {
				err = -EOPNOTSUPP;
				goto err_ct;
			}
			rcu_read_lock();
			helper = __nf_conntrack_helper_find(helpname, u3,
							    nf_ct_protonum(ct));
			if (helper) {
				err = -EAGAIN;
				goto err_rcu;
			}
			rcu_read_unlock();
#endif
			err = -EOPNOTSUPP;
			goto err_ct;
		}
	}

	exp = ctnetlink_alloc_expect(cda, ct, helper, &tuple, &mask);
	if (IS_ERR(exp)) {
		err = PTR_ERR(exp);
		goto err_rcu;
	}

	err = nf_ct_expect_related_report(exp, portid, report, 0);
	nf_ct_expect_put(exp);
err_rcu:
	rcu_read_unlock();
err_ct:
	nf_ct_put(ct);
	return err;
}