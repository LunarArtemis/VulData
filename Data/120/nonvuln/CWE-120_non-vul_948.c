static int32_t dmar_register_hrhd(struct dmar_drhd_rt *dmar_unit)
{
	int32_t ret = 0;

	dev_dbg(DBG_LEVEL_IOMMU, "Register dmar uint [%d] @0x%lx", dmar_unit->index, dmar_unit->drhd->reg_base_addr);

	spinlock_init(&dmar_unit->lock);

	dmar_unit->cap = iommu_read64(dmar_unit, DMAR_CAP_REG);
	dmar_unit->ecap = iommu_read64(dmar_unit, DMAR_ECAP_REG);

	/*
	 * The initialization of "dmar_unit->gcmd" shall be done via reading from Global Status Register rather than
	 * Global Command Register.
	 * According to Chapter 10.4.4 Global Command Register in VT-d spec, Global Command Register is a write-only
	 * register to control remapping hardware. Global Status Register is the corresponding read-only register to
	 * report remapping hardware status.
	 */
	dmar_unit->gcmd = iommu_read32(dmar_unit, DMAR_GSTS_REG);

	dmar_unit->cap_msagaw = dmar_unit_get_msagw(dmar_unit);

	dmar_unit->cap_num_fault_regs = iommu_cap_num_fault_regs(dmar_unit->cap);
	dmar_unit->cap_fault_reg_offset = iommu_cap_fault_reg_offset(dmar_unit->cap);
	dmar_unit->ecap_iotlb_offset = iommu_ecap_iro(dmar_unit->ecap) * 16U;
	dmar_unit->root_table_addr = hva2hpa(get_root_table(dmar_unit->index));
	dmar_unit->ir_table_addr = hva2hpa(get_ir_table(dmar_unit->index));

#if DBG_IOMMU
	pr_info("version:0x%x, cap:0x%lx, ecap:0x%lx",
		iommu_read32(dmar_unit, DMAR_VER_REG), dmar_unit->cap, dmar_unit->ecap);
	pr_info("sagaw:0x%x, msagaw:0x%x, iotlb offset 0x%x",
		iommu_cap_sagaw(dmar_unit->cap), dmar_unit->cap_msagaw, dmar_unit->ecap_iotlb_offset);

	dmar_unit_show_capability(dmar_unit);
#endif

	/* check capability */
	if ((iommu_cap_super_page_val(dmar_unit->cap) & 0x1U) == 0U) {
		pr_fatal("%s: dmar uint doesn't support 2MB page!\n", __func__);
		ret = -ENODEV;
	} else if ((iommu_cap_super_page_val(dmar_unit->cap) & 0x2U) == 0U) {
		pr_fatal("%s: dmar uint doesn't support 1GB page!\n", __func__);
		ret = -ENODEV;
	} else if (iommu_ecap_qi(dmar_unit->ecap) == 0U) {
		pr_fatal("%s: dmar unit doesn't support Queued Invalidation!", __func__);
		ret = -ENODEV;
	} else if (iommu_ecap_ir(dmar_unit->ecap) == 0U) {
		pr_fatal("%s: dmar unit doesn't support Interrupt Remapping!", __func__);
		ret = -ENODEV;
	} else if (iommu_ecap_eim(dmar_unit->ecap) == 0U) {
		pr_fatal("%s: dmar unit doesn't support Extended Interrupt Mode!", __func__);
		ret = -ENODEV;
	} else {
		if ((iommu_ecap_c(dmar_unit->ecap) == 0U) && (!dmar_unit->drhd->ignore)) {
			iommu_page_walk_coherent = false;
		}
		dmar_disable_translation(dmar_unit);
	}

	return ret;
}