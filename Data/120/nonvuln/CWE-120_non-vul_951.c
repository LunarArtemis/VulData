static inline uint64_t dmar_get_bitslice(uint64_t var, uint64_t mask, uint32_t pos)
{
	return ((var & mask) >> pos);
}