static inline size_t ctnetlink_acct_size(const struct nf_conn *ct)
{
	if (!nf_ct_ext_exist(ct, NF_CT_EXT_ACCT))
		return 0;
	return 2 * nla_total_size(0) /* CTA_COUNTERS_ORIG|REPL */
	       + 2 * nla_total_size_64bit(sizeof(uint64_t)) /* CTA_COUNTERS_PACKETS */
	       + 2 * nla_total_size_64bit(sizeof(uint64_t)) /* CTA_COUNTERS_BYTES */
	       ;
}