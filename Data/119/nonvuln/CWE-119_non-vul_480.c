static inline void
fc_dma_sync_single_for_cpu(struct device *dev, dma_addr_t addr, size_t size,
		enum dma_data_direction dir)
{
	if (dev)
		dma_sync_single_for_cpu(dev, addr, size, dir);
}