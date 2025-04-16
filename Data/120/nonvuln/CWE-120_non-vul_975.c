static inline uint8_t *get_ctx_table(uint32_t dmar_index, uint8_t bus_no)
{
	static struct context_table ctx_tables[CONFIG_MAX_IOMMU_NUM] __aligned(PAGE_SIZE);
	return ctx_tables[dmar_index].buses[bus_no].contents;
}