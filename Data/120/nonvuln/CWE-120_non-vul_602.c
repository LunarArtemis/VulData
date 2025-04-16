static inline int ctnetlink_label_size(const struct nf_conn *ct)
{
	struct nf_conn_labels *labels = nf_ct_labels_find(ct);

	if (!labels)
		return 0;
	return nla_total_size(sizeof(labels->bits));
}