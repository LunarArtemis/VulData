static inline uint8_t width_to_agaw(uint32_t width)
{
	return width_to_level(width) - 2U;
}