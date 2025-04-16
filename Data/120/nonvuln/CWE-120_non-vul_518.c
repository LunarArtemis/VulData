static uint32_t
mac_readreg(E1000State *s, int index)
{
    return s->mac_reg[index];
}