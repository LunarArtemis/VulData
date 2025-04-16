static inline uint64_t dmar_set_bitslice(uint64_t var, uint64_t mask, uint32_t pos, uint64_t val)
{
	return ((var & ~mask) | ((val << pos) & mask));
}