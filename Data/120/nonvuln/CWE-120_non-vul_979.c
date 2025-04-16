static inline uint8_t iommu_ecap_sc(uint64_t ecap)
{
	return ((uint8_t)(ecap >> 7U) & 1U);
}