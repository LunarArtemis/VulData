static void
set_interrupt_cause(E1000State *s, int index, uint32_t val)
{
    if (val && (E1000_DEVID >= E1000_DEV_ID_82547EI_MOBILE)) {
        /* Only for 8257x */
        val |= E1000_ICR_INT_ASSERTED;
    }
    s->mac_reg[ICR] = val;
    s->mac_reg[ICS] = val;
    qemu_set_irq(s->dev.irq[0], (s->mac_reg[IMS] & s->mac_reg[ICR]) != 0);
}