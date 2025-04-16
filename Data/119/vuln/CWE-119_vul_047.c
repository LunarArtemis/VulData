static int spl2sw_nvmem_get_mac_address(struct device *dev, struct device_node *np,
					void *addrbuf)
{
	struct nvmem_cell *cell;
	ssize_t len;
	u8 *mac;

	/* Get nvmem cell of mac-address from dts. */
	cell = of_nvmem_cell_get(np, "mac-address");
	if (IS_ERR(cell))
		return PTR_ERR(cell);

	/* Read mac address from nvmem cell. */
	mac = nvmem_cell_read(cell, &len);
	nvmem_cell_put(cell);
	if (IS_ERR(mac))
		return PTR_ERR(mac);

	if (len != ETH_ALEN) {
		kfree(mac);
		dev_info(dev, "Invalid length of mac address in nvmem!\n");
		return -EINVAL;
	}

	/* Byte order of some samples are reversed.
	 * Convert byte order here.
	 */
	spl2sw_check_mac_vendor_id_and_convert(mac);

	/* Check if mac address is valid */
	if (!is_valid_ether_addr(mac)) {
		kfree(mac);
		dev_info(dev, "Invalid mac address in nvmem (%pM)!\n", mac);
		return -EINVAL;
	}

	ether_addr_copy(addrbuf, mac);
	kfree(mac);
	return 0;
}