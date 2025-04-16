struct iommu_domain *create_iommu_domain(uint16_t vm_id, uint64_t translation_table, uint32_t addr_width)
{
	static struct iommu_domain iommu_domains[MAX_DOMAIN_NUM];
	struct iommu_domain *domain;

	/* TODO: check if a domain with the vm_id exists */

	if (translation_table == 0UL) {
		pr_err("translation table is NULL");
		domain = NULL;
	} else {
		/*
		 * A hypercall is called to create an iommu domain for a valid VM,
		 * and hv code limit the VM number to CONFIG_MAX_VM_NUM.
		 * So the array iommu_domains will not be accessed out of range.
		 */
		domain = &iommu_domains[vmid_to_domainid(vm_id)];

		domain->vm_id = vm_id;
		domain->trans_table_ptr = translation_table;
		domain->addr_width = addr_width;

		dev_dbg(DBG_LEVEL_IOMMU, "create domain [%d]: vm_id = %hu, ept@0x%x",
			vmid_to_domainid(domain->vm_id), domain->vm_id, domain->trans_table_ptr);
	}

	return domain;
}