static void
mac_writereg(E1000State *s, int index, uint32_t val)
{
    s->mac_reg[index] = val;
}