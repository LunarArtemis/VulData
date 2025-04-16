static struct dmar_drhd_rt *ioapic_to_dmaru(uint16_t ioapic_id, union pci_bdf *sid)
{
	struct dmar_drhd_rt *dmar_unit = NULL;
	uint32_t i, j;
	bool found = false;

	for (j = 0U; j < platform_dmar_info->drhd_count; j++) {
		dmar_unit = &dmar_drhd_units[j];
		for (i = 0U; i < dmar_unit->drhd->dev_cnt; i++) {
			if ((dmar_unit->drhd->devices[i].type == ACPI_DMAR_SCOPE_TYPE_IOAPIC) &&
					(dmar_unit->drhd->devices[i].id == ioapic_id)) {
				sid->fields.devfun = dmar_unit->drhd->devices[i].devfun;
				sid->fields.bus = dmar_unit->drhd->devices[i].bus;
				found = true;
				break;
			}
		}
		if (found) {
			break;
		}
	}

	if (j == platform_dmar_info->drhd_count) {
		dmar_unit = NULL;
	}

	return dmar_unit;
}