uint32_t faad_get_processed_bits(bitfile *ld)
{
    return (uint32_t)(8 * (4*(ld->tail - ld->start) - 4) - (ld->bits_left));
}