static inline int
fc_dma_map_sg(struct device *dev, struct scatterlist *sg, int nents,
		enum dma_data_direction dir)
{
	return dev ? dma_map_sg(dev, sg, nents, dir) : fc_map_sg(sg, nents);
}