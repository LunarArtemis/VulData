static int ctnetlink_filter_match(struct nf_conn *ct, void *data)
{
	struct ctnetlink_filter *filter = data;
	struct nf_conntrack_tuple *tuple;

	if (filter == NULL)
		goto out;

	/* Match entries of a given L3 protocol number.
	 * If it is not specified, ie. l3proto == 0,
	 * then match everything.
	 */
	if (filter->family && nf_ct_l3num(ct) != filter->family)
		goto ignore_entry;

	if (filter->orig_flags) {
		tuple = nf_ct_tuple(ct, IP_CT_DIR_ORIGINAL);
		if (!ctnetlink_filter_match_tuple(&filter->orig, tuple,
						  filter->orig_flags,
						  filter->family))
			goto ignore_entry;
	}

	if (filter->reply_flags) {
		tuple = nf_ct_tuple(ct, IP_CT_DIR_REPLY);
		if (!ctnetlink_filter_match_tuple(&filter->reply, tuple,
						  filter->reply_flags,
						  filter->family))
			goto ignore_entry;
	}

#ifdef CONFIG_NF_CONNTRACK_MARK
	if ((filter->cta_flags & CTA_FILTER_FLAG(CTA_MARK_MASK)) &&
	    (ct->mark & filter->mark.mask) != filter->mark.val)
		goto ignore_entry;
	else if ((filter->cta_flags & CTA_FILTER_FLAG(CTA_MARK)) &&
		 ct->mark != filter->mark.val)
		goto ignore_entry;
#endif

out:
	return 1;

ignore_entry:
	return 0;
}