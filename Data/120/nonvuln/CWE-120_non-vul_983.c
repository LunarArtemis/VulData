int32_t move_pt_device(const struct iommu_domain *from_domain, const struct iommu_domain *to_domain, uint8_t bus, uint8_t devfun)
{
	int32_t status = 0;
	uint16_t bus_local = bus;

	/* TODO: check if the device assigned */

	if (bus_local < CONFIG_IOMMU_BUS_NUM) {
		if (from_domain != NULL) {
			status = iommu_detach_device(from_domain, bus, devfun);
		}

		if ((status == 0) && (to_domain != NULL)) {
			status = iommu_attach_device(to_domain, bus, devfun);
		}
	} else {
		status = -EINVAL;
	}

	return status;
}