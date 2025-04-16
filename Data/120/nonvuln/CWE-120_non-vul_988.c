static bool is_dmar_unit_valid(const struct dmar_drhd_rt *dmar_unit, union pci_bdf sid)
{
	bool valid = false;

	if (dmar_unit == NULL) {
		pr_err("no dmar unit found for device: %x:%x.%x", sid.bits.b, sid.bits.d, sid.bits.f);
	} else if (dmar_unit->drhd->ignore) {
		dev_dbg(DBG_LEVEL_IOMMU, "device is ignored : %x:%x.%x", sid.bits.b, sid.bits.d, sid.bits.f);
	} else {
		valid = true;
	}

	return valid;
}