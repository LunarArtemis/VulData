static int32_t iommu_attach_device(const struct iommu_domain *domain, uint8_t bus, uint8_t devfun)
{
	struct dmar_drhd_rt *dmar_unit;
	struct dmar_entry *root_table;
	uint64_t context_table_addr;
	struct dmar_entry *context;
	struct dmar_entry *root_entry;
	struct dmar_entry *context_entry;
	uint64_t hi_64 = 0UL;
	uint64_t lo_64 = 0UL;
	int32_t ret = -EINVAL;
	/* source id */
	union pci_bdf sid;

	sid.fields.bus = bus;
	sid.fields.devfun = devfun;

	dmar_unit = device_to_dmaru(bus, devfun);
	if (is_dmar_unit_valid(dmar_unit, sid) && dmar_unit_support_aw(dmar_unit, domain->addr_width)) {
		root_table = (struct dmar_entry *)hpa2hva(dmar_unit->root_table_addr);
		root_entry = root_table + bus;

		if (dmar_get_bitslice(root_entry->lo_64,
					ROOT_ENTRY_LOWER_PRESENT_MASK,
					ROOT_ENTRY_LOWER_PRESENT_POS) == 0UL) {
			/* create context table for the bus if not present */
			context_table_addr = hva2hpa(get_ctx_table(dmar_unit->index, bus));

			context_table_addr = context_table_addr >> PAGE_SHIFT;

			lo_64 = dmar_set_bitslice(lo_64,
					ROOT_ENTRY_LOWER_CTP_MASK, ROOT_ENTRY_LOWER_CTP_POS, context_table_addr);
			lo_64 = dmar_set_bitslice(lo_64,
					ROOT_ENTRY_LOWER_PRESENT_MASK, ROOT_ENTRY_LOWER_PRESENT_POS, 1UL);

			root_entry->hi_64 = 0UL;
			root_entry->lo_64 = lo_64;
			iommu_flush_cache(root_entry, sizeof(struct dmar_entry));
		} else {
			context_table_addr = dmar_get_bitslice(root_entry->lo_64,
					ROOT_ENTRY_LOWER_CTP_MASK, ROOT_ENTRY_LOWER_CTP_POS);
		}

		context_table_addr = context_table_addr << PAGE_SHIFT;

		context = (struct dmar_entry *)hpa2hva(context_table_addr);
		context_entry = context + devfun;

		if (dmar_get_bitslice(context_entry->lo_64, CTX_ENTRY_LOWER_P_MASK, CTX_ENTRY_LOWER_P_POS) != 0UL) {
			/* the context entry should not be present */
			pr_err("%s: context entry@0x%lx (Lower:%x) ", __func__, context_entry, context_entry->lo_64);
			pr_err("already present for %x:%x.%x", bus, sid.bits.d, sid.bits.f);
			ret = -EBUSY;
		} else {
			/* setup context entry for the devfun */
			/* TODO: add Device TLB support */
			hi_64 = dmar_set_bitslice(hi_64, CTX_ENTRY_UPPER_AW_MASK, CTX_ENTRY_UPPER_AW_POS,
					(uint64_t)width_to_agaw(domain->addr_width));
			lo_64 = dmar_set_bitslice(lo_64, CTX_ENTRY_LOWER_TT_MASK, CTX_ENTRY_LOWER_TT_POS,
					DMAR_CTX_TT_UNTRANSLATED);
			hi_64 = dmar_set_bitslice(hi_64, CTX_ENTRY_UPPER_DID_MASK, CTX_ENTRY_UPPER_DID_POS,
				(uint64_t)vmid_to_domainid(domain->vm_id));
			lo_64 = dmar_set_bitslice(lo_64, CTX_ENTRY_LOWER_SLPTPTR_MASK, CTX_ENTRY_LOWER_SLPTPTR_POS,
				domain->trans_table_ptr >> PAGE_SHIFT);
			lo_64 = dmar_set_bitslice(lo_64, CTX_ENTRY_LOWER_P_MASK, CTX_ENTRY_LOWER_P_POS, 1UL);

			context_entry->hi_64 = hi_64;
			context_entry->lo_64 = lo_64;
			iommu_flush_cache(context_entry, sizeof(struct dmar_entry));
			ret = 0;
		}
	} else if (is_dmar_unit_ignored(dmar_unit)) {
	       ret = 0;
	}

	return ret;
}