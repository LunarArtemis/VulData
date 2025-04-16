static void dmar_issue_qi_request(struct dmar_drhd_rt *dmar_unit, struct dmar_entry invalidate_desc)
{
	struct dmar_entry *invalidate_desc_ptr;
	uint32_t qi_status = 0U;
	uint64_t start;

	spinlock_obtain(&(dmar_unit->lock));

	invalidate_desc_ptr = (struct dmar_entry *)(dmar_unit->qi_queue + dmar_unit->qi_tail);

	invalidate_desc_ptr->hi_64 = invalidate_desc.hi_64;
	invalidate_desc_ptr->lo_64 = invalidate_desc.lo_64;
	dmar_unit->qi_tail = (dmar_unit->qi_tail + DMAR_QI_INV_ENTRY_SIZE) % DMAR_INVALIDATION_QUEUE_SIZE;

	invalidate_desc_ptr++;

	invalidate_desc_ptr->hi_64 = hva2hpa(&qi_status);
	invalidate_desc_ptr->lo_64 = DMAR_INV_WAIT_DESC_LOWER;
	dmar_unit->qi_tail = (dmar_unit->qi_tail + DMAR_QI_INV_ENTRY_SIZE) % DMAR_INVALIDATION_QUEUE_SIZE;

	qi_status = DMAR_INV_STATUS_INCOMPLETE;
	iommu_write32(dmar_unit, DMAR_IQT_REG, dmar_unit->qi_tail);

	start = cpu_ticks();
	while (qi_status != DMAR_INV_STATUS_COMPLETED) {
		if ((cpu_ticks() - start) > TICKS_PER_MS) {
			pr_err("DMAR OP Timeout! @ %s", __func__);
			break;
		}
		asm_pause();
	}

	spinlock_release(&(dmar_unit->lock));
}