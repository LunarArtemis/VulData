static int
ctnetlink_dump_labels(struct sk_buff *skb, const struct nf_conn *ct)
{
	struct nf_conn_labels *labels = nf_ct_labels_find(ct);
	unsigned int i;

	if (!labels)
		return 0;

	i = 0;
	do {
		if (labels->bits[i] != 0)
			return nla_put(skb, CTA_LABELS, sizeof(labels->bits),
				       labels->bits);
		i++;
	} while (i < ARRAY_SIZE(labels->bits));

	return 0;
}