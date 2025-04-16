static struct dmar_drhd_rt *device_to_dmaru(uint8_t bus, uint8_t devfun)
{
	struct dmar_drhd_rt *dmaru = NULL;
	uint16_t bdf = ((uint16_t)bus << 8U) | devfun;
	uint32_t index = pci_lookup_drhd_for_pbdf(bdf);

	if (index == INVALID_DRHD_INDEX) {
		pr_fatal("BDF %02x:%02x:%x has no IOMMU\n", bus, devfun >> 3U, devfun & 7U);
		/*
		 * pci_lookup_drhd_for_pbdf would return -1U for any of the reasons
		 * 1) PCI device with bus, devfun does not exist on platform
		 * 2) ACRN had issues finding the device with bus, devfun during init
		 * 3) DMAR tables provided by ACPI for this platform are incorrect
		 */
	} else {
		dmaru = &dmar_drhd_units[index];
	}

	return dmaru;
}