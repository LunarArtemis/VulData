static void dmar_invalid_iotlb(struct dmar_drhd_rt *dmar_unit, uint16_t did, uint64_t address, uint8_t am,
			       bool hint, enum dmar_iirg_type iirg)
{
	/* set Drain Reads & Drain Writes,
	 * if hardware doesn't support it, will be ignored by hardware
	 */
	struct dmar_entry invalidate_desc;
	uint64_t addr = 0UL;

	invalidate_desc.hi_64 = 0UL;

	invalidate_desc.lo_64 = DMA_IOTLB_DR | DMA_IOTLB_DW | DMAR_INV_IOTLB_DESC;

	switch (iirg) {
	case DMAR_IIRG_GLOBAL:
		invalidate_desc.lo_64 |= DMA_IOTLB_GLOBAL_INVL;
		break;
	case DMAR_IIRG_DOMAIN:
		invalidate_desc.lo_64 |= DMA_IOTLB_DOMAIN_INVL | dma_iotlb_did(did);
		break;
	case DMAR_IIRG_PAGE:
		invalidate_desc.lo_64 |= DMA_IOTLB_PAGE_INVL | dma_iotlb_did(did);
		addr = address | dma_iotlb_invl_addr_am(am);
		if (hint) {
			addr |= DMA_IOTLB_INVL_ADDR_IH_UNMODIFIED;
		}
		invalidate_desc.hi_64 |= addr;
		break;
	default:
		invalidate_desc.lo_64 = 0UL;
		pr_err("unknown IIRG type");
	}

	if (invalidate_desc.lo_64 != 0UL) {
		dmar_issue_qi_request(dmar_unit, invalidate_desc);
	}
}