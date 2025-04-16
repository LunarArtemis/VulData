int netlbl_cipsov4_genl_init(void)
{
	int ret_val;

	ret_val = genl_register_family(&netlbl_cipsov4_gnl_family);
	if (ret_val != 0)
		return ret_val;

	ret_val = genl_register_ops(&netlbl_cipsov4_gnl_family,
				    &netlbl_cipsov4_genl_c_add);
	if (ret_val != 0)
		return ret_val;
	ret_val = genl_register_ops(&netlbl_cipsov4_gnl_family,
				    &netlbl_cipsov4_genl_c_remove);
	if (ret_val != 0)
		return ret_val;
	ret_val = genl_register_ops(&netlbl_cipsov4_gnl_family,
				    &netlbl_cipsov4_genl_c_list);
	if (ret_val != 0)
		return ret_val;
	ret_val = genl_register_ops(&netlbl_cipsov4_gnl_family,
				    &netlbl_cipsov4_genl_c_listall);
	if (ret_val != 0)
		return ret_val;

	return 0;
}