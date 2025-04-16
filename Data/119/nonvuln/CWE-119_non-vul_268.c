static int netlbl_cipsov4_listall_cb(struct cipso_v4_doi *doi_def, void *arg)
{
	int ret_val = -ENOMEM;
	struct netlbl_cipsov4_doiwalk_arg *cb_arg = arg;
	void *data;

	data = genlmsg_put(cb_arg->skb, NETLINK_CB(cb_arg->nl_cb->skb).pid,
			   cb_arg->seq, &netlbl_cipsov4_gnl_family,
			   NLM_F_MULTI, NLBL_CIPSOV4_C_LISTALL);
	if (data == NULL)
		goto listall_cb_failure;

	ret_val = nla_put_u32(cb_arg->skb, NLBL_CIPSOV4_A_DOI, doi_def->doi);
	if (ret_val != 0)
		goto listall_cb_failure;
	ret_val = nla_put_u32(cb_arg->skb,
			      NLBL_CIPSOV4_A_MTYPE,
			      doi_def->type);
	if (ret_val != 0)
		goto listall_cb_failure;

	return genlmsg_end(cb_arg->skb, data);

listall_cb_failure:
	genlmsg_cancel(cb_arg->skb, data);
	return ret_val;
}