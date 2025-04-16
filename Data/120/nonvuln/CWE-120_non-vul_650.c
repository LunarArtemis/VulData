static int
dump_counters(struct sk_buff *skb, struct nf_conn_acct *acct,
	      enum ip_conntrack_dir dir, int type)
{
	enum ctattr_type attr = dir ? CTA_COUNTERS_REPLY: CTA_COUNTERS_ORIG;
	struct nf_conn_counter *counter = acct->counter;
	struct nlattr *nest_count;
	u64 pkts, bytes;

	if (type == IPCTNL_MSG_CT_GET_CTRZERO) {
		pkts = atomic64_xchg(&counter[dir].packets, 0);
		bytes = atomic64_xchg(&counter[dir].bytes, 0);
	} else {
		pkts = atomic64_read(&counter[dir].packets);
		bytes = atomic64_read(&counter[dir].bytes);
	}

	nest_count = nla_nest_start(skb, attr);
	if (!nest_count)
		goto nla_put_failure;

	if (nla_put_be64(skb, CTA_COUNTERS_PACKETS, cpu_to_be64(pkts),
			 CTA_COUNTERS_PAD) ||
	    nla_put_be64(skb, CTA_COUNTERS_BYTES, cpu_to_be64(bytes),
			 CTA_COUNTERS_PAD))
		goto nla_put_failure;

	nla_nest_end(skb, nest_count);

	return 0;

nla_put_failure:
	return -1;
}