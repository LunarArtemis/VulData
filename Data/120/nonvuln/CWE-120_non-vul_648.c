static __be32 nf_expect_get_id(const struct nf_conntrack_expect *exp)
{
	static __read_mostly siphash_key_t exp_id_seed;
	unsigned long a, b, c, d;

	net_get_random_once(&exp_id_seed, sizeof(exp_id_seed));

	a = (unsigned long)exp;
	b = (unsigned long)exp->helper;
	c = (unsigned long)exp->master;
	d = (unsigned long)siphash(&exp->tuple, sizeof(exp->tuple), &exp_id_seed);

#ifdef CONFIG_64BIT
	return (__force __be32)siphash_4u64((u64)a, (u64)b, (u64)c, (u64)d, &exp_id_seed);
#else
	return (__force __be32)siphash_4u32((u32)a, (u32)b, (u32)c, (u32)d, &exp_id_seed);
#endif
}