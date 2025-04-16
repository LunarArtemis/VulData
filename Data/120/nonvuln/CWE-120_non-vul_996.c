static inline void *get_qi_queue(uint32_t dmar_index)
{
	static struct page qi_queues[CONFIG_MAX_IOMMU_NUM] __aligned(PAGE_SIZE);
	return (void *)qi_queues[dmar_index].contents;
}