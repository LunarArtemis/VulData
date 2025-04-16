static int
ctnetlink_glue_parse_ct(const struct nlattr *cda[], struct nf_conn *ct)
{
	int err;

	if (cda[CTA_TIMEOUT]) {
		err = ctnetlink_change_timeout(ct, cda);
		if (err < 0)
			return err;
	}
	if (cda[CTA_STATUS]) {
		err = ctnetlink_update_status(ct, cda);
		if (err < 0)
			return err;
	}
	if (cda[CTA_HELP]) {
		err = ctnetlink_change_helper(ct, cda);
		if (err < 0)
			return err;
	}
	if (cda[CTA_LABELS]) {
		err = ctnetlink_attach_labels(ct, cda);
		if (err < 0)
			return err;
	}
#if defined(CONFIG_NF_CONNTRACK_MARK)
	if (cda[CTA_MARK]) {
		ctnetlink_change_mark(ct, cda);
	}
#endif
	return 0;
}