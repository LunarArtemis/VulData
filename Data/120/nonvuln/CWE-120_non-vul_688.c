static int
ctnetlink_attach_labels(struct nf_conn *ct, const struct nlattr * const cda[])
{
#ifdef CONFIG_NF_CONNTRACK_LABELS
	size_t len = nla_len(cda[CTA_LABELS]);
	const void *mask = cda[CTA_LABELS_MASK];

	if (len & (sizeof(u32)-1)) /* must be multiple of u32 */
		return -EINVAL;

	if (mask) {
		if (nla_len(cda[CTA_LABELS_MASK]) == 0 ||
		    nla_len(cda[CTA_LABELS_MASK]) != len)
			return -EINVAL;
		mask = nla_data(cda[CTA_LABELS_MASK]);
	}

	len /= sizeof(u32);

	return nf_connlabels_replace(ct, nla_data(cda[CTA_LABELS]), mask, len);
#else
	return -EOPNOTSUPP;
#endif
}