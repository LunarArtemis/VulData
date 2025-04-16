static int32_t iommu_detach_device(const struct iommu_domain *domain, uint8_t bus, uint8_t devfun)
{
	struct dmar_drhd_rt *dmar_unit;
	struct dmar_entry *root_table;
	uint64_t context_table_addr;
	struct dmar_entry *context;
	struct dmar_entry *root_entry;
	struct dmar_entry *context_entry;
	/* source id */
	union pci_bdf sid;
	int32_t ret = -EINVAL;

	dmar_unit = device_to_dmaru(bus, devfun);

	sid.fields.bus = bus;
	sid.fields.devfun = devfun;

	if (is_dmar_unit_valid(dmar_unit, sid)) {
		root_table = (struct dmar_entry *)hpa2hva(dmar_unit->root_table_addr);
		root_entry = root_table + bus;
		ret = 0;

		context_table_addr = dmar_get_bitslice(root_entry->lo_64,  ROOT_ENTRY_LOWER_CTP_MASK,
							ROOT_ENTRY_LOWER_CTP_POS);
		context_table_addr = context_table_addr << PAGE_SHIFT;
		context = (struct dmar_entry *)hpa2hva(context_table_addr);

		context_entry = context + devfun;

		if ((context == NULL) || (context_entry == NULL)) {
			pr_err("dmar context entry is invalid");
			ret = -EINVAL;
		} else if ((uint16_t)dmar_get_bitslice(context_entry->hi_64, CTX_ENTRY_UPPER_DID_MASK,
						CTX_ENTRY_UPPER_DID_POS) != vmid_to_domainid(domain->vm_id)) {
			pr_err("%s: domain id mismatch", __func__);
			ret = -EPERM;
		} else {
			/* clear the present bit first */
			context_entry->lo_64 = 0UL;
			context_entry->hi_64 = 0UL;
			iommu_flush_cache(context_entry, sizeof(struct dmar_entry));

			dmar_invalid_context_cache(dmar_unit, vmid_to_domainid(domain->vm_id), sid.value, 0U,
							DMAR_CIRG_DEVICE);
			dmar_invalid_iotlb(dmar_unit, vmid_to_domainid(domain->vm_id), 0UL, 0U, false,
							DMAR_IIRG_DOMAIN);
		}
	} else if (is_dmar_unit_ignored(dmar_unit)) {
	       ret = 0;
	}

	return ret;
}