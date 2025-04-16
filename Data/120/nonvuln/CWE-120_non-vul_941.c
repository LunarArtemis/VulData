static void dmar_invalid_iec(struct dmar_drhd_rt *dmar_unit, uint16_t intr_index,
				uint8_t index_mask, bool is_global)
{
	struct dmar_entry invalidate_desc;

	invalidate_desc.hi_64 = 0UL;
	invalidate_desc.lo_64 = DMAR_INV_IEC_DESC;

	if (is_global) {
		invalidate_desc.lo_64 |= DMAR_IEC_GLOBAL_INVL;
	} else {
		invalidate_desc.lo_64 |= DMAR_IECI_INDEXED | dma_iec_index(intr_index, index_mask);
	}

	if (invalidate_desc.lo_64 != 0UL) {
		dmar_issue_qi_request(dmar_unit, invalidate_desc);
	}
}