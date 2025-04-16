static int netlbl_cipsov4_remove(struct sk_buff *skb, struct genl_info *info)
{
	int ret_val = -EINVAL;
	u32 doi = 0;
	struct audit_buffer *audit_buf;
	struct netlbl_audit audit_info;

	if (!info->attrs[NLBL_CIPSOV4_A_DOI])
		return -EINVAL;

	doi = nla_get_u32(info->attrs[NLBL_CIPSOV4_A_DOI]);
	netlbl_netlink_auditinfo(skb, &audit_info);

	ret_val = cipso_v4_doi_remove(doi,
				      &audit_info,
				      netlbl_cipsov4_doi_free);

	audit_buf = netlbl_audit_start_common(AUDIT_MAC_CIPSOV4_DEL,
					      &audit_info);
	if (audit_buf != NULL) {
		audit_log_format(audit_buf,
				 " cipso_doi=%u res=%u",
				 doi,
				 ret_val == 0 ? 1 : 0);
		audit_log_end(audit_buf);
	}

	return ret_val;
}