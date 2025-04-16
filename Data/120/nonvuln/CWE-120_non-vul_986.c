static inline uint8_t iommu_cap_rwbf(uint64_t cap)
{
	return ((uint8_t)(cap >> 4U) & 1U);
}