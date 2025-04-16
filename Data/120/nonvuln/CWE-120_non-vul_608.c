static struct nf_conn *ctnetlink_glue_get_ct(const struct sk_buff *skb,
					     enum ip_conntrack_info *ctinfo)
{
	return nf_ct_get(skb, ctinfo);
}