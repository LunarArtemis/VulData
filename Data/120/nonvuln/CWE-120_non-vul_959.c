void iommu_flush_cache(const void *p, uint32_t size)
{
	/* if vtd support page-walk coherency, no need to flush cacheline */
	if (!iommu_page_walk_coherent) {
		flush_cache_range(p, size);
	}
}