static inline void *get_ir_table(uint32_t dmar_index)
{
	static struct intr_remap_table ir_tables[CONFIG_MAX_IOMMU_NUM] __aligned(PAGE_SIZE);
	return (void *)ir_tables[dmar_index].tables[0].contents;
}