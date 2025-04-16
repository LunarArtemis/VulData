static void dmar_invalid_context_cache(struct dmar_drhd_rt *dmar_unit,
	uint16_t did, uint16_t sid, uint8_t fm, enum dmar_cirg_type cirg)
{
	struct dmar_entry invalidate_desc;

	invalidate_desc.hi_64 = 0UL;
	invalidate_desc.lo_64 = DMAR_INV_CONTEXT_CACHE_DESC;
	switch (cirg) {
	case DMAR_CIRG_GLOBAL:
		invalidate_desc.lo_64 |= DMA_CONTEXT_GLOBAL_INVL;
		break;
	case DMAR_CIRG_DOMAIN:
		invalidate_desc.lo_64 |= DMA_CONTEXT_DOMAIN_INVL | dma_ccmd_did(did);
		break;
	case DMAR_CIRG_DEVICE:
		invalidate_desc.lo_64 |= DMA_CONTEXT_DEVICE_INVL | dma_ccmd_did(did) | dma_ccmd_sid(sid) | dma_ccmd_fm(fm);
		break;
	default:
		invalidate_desc.lo_64 = 0UL;
		pr_err("unknown CIRG type");
		break;
	}

	if (invalidate_desc.lo_64 != 0UL) {
		dmar_issue_qi_request(dmar_unit, invalidate_desc);
	}
}