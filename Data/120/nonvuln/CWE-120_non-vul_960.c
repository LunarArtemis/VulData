static void dmar_fault_handler(uint32_t irq, void *data)
{
	struct dmar_drhd_rt *dmar_unit = (struct dmar_drhd_rt *)data;
	uint32_t fsr;
	uint32_t index;
	uint32_t record_reg_offset;
	struct dmar_entry fault_record;
	int32_t loop = 0;

	dev_dbg(DBG_LEVEL_IOMMU, "%s: irq = %d", __func__, irq);

	fsr = iommu_read32(dmar_unit, DMAR_FSTS_REG);

#if DBG_IOMMU
	fault_status_analysis(fsr);
#endif

	while (dma_fsts_ppf(fsr)) {
		loop++;
		index = dma_fsts_fri(fsr);
		record_reg_offset = (uint32_t)dmar_unit->cap_fault_reg_offset + (index * 16U);
		if (index >= dmar_unit->cap_num_fault_regs) {
			dev_dbg(DBG_LEVEL_IOMMU, "%s: invalid FR Index", __func__);
			break;
		}

		/* read 128-bit fault recording register */
		fault_record.lo_64 = iommu_read64(dmar_unit, record_reg_offset);
		fault_record.hi_64 = iommu_read64(dmar_unit, record_reg_offset + 8U);

		dev_dbg(DBG_LEVEL_IOMMU, "%s: record[%d] @0x%x:  0x%lx, 0x%lx",
			__func__, index, record_reg_offset, fault_record.lo_64, fault_record.hi_64);

		fault_record_analysis(fault_record.lo_64, fault_record.hi_64);

		/* write to clear */
		iommu_write64(dmar_unit, record_reg_offset, fault_record.lo_64);
		iommu_write64(dmar_unit, record_reg_offset + 8U, fault_record.hi_64);

#ifdef DMAR_FAULT_LOOP_MAX
		if (loop > DMAR_FAULT_LOOP_MAX) {
			dev_dbg(DBG_LEVEL_IOMMU, "%s: loop more than %d times", __func__, DMAR_FAULT_LOOP_MAX);
			break;
		}
#endif

		fsr = iommu_read32(dmar_unit, DMAR_FSTS_REG);
	}
}