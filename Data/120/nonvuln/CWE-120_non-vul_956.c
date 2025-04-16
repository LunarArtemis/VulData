static bool is_irte_reserved(const struct dmar_drhd_rt *dmar_unit, uint16_t index)
{
	return ((dmar_unit->irte_reserved_bitmap[index >> 6U] & (1UL << (index & 0x3FU))) != 0UL);
}