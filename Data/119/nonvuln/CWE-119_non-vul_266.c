static int netlbl_cipsov4_add(struct sk_buff *skb, struct genl_info *info)

{
	int ret_val = -EINVAL;
	u32 type;
	u32 doi;
	const char *type_str = "(unknown)";
	struct audit_buffer *audit_buf;
	struct netlbl_audit audit_info;

	if (!info->attrs[NLBL_CIPSOV4_A_DOI] ||
	    !info->attrs[NLBL_CIPSOV4_A_MTYPE])
		return -EINVAL;

	doi = nla_get_u32(info->attrs[NLBL_CIPSOV4_A_DOI]);
	netlbl_netlink_auditinfo(skb, &audit_info);

	type = nla_get_u32(info->attrs[NLBL_CIPSOV4_A_MTYPE]);
	switch (type) {
	case CIPSO_V4_MAP_STD:
		type_str = "std";
		ret_val = netlbl_cipsov4_add_std(info);
		break;
	case CIPSO_V4_MAP_PASS:
		type_str = "pass";
		ret_val = netlbl_cipsov4_add_pass(info);
		break;
	}

	audit_buf = netlbl_audit_start_common(AUDIT_MAC_CIPSOV4_ADD,
					      &audit_info);
	if (audit_buf != NULL) {
		audit_log_format(audit_buf,
				 " cipso_doi=%u cipso_type=%s res=%u",
				 doi,
				 type_str,
				 ret_val == 0 ? 1 : 0);
		audit_log_end(audit_buf);
	}

	return ret_val;
}