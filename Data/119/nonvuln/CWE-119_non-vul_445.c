static inline int
fc_dma_mapping_error(struct device *dev, dma_addr_t dma_addr)
{
	return dev ? dma_mapping_error(dev, dma_addr) : 0;
}