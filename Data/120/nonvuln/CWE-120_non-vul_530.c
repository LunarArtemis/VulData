static int
e1000_can_receive(NetClientState *nc)
{
    E1000State *s = DO_UPCAST(NICState, nc, nc)->opaque;

    return (s->mac_reg[RCTL] & E1000_RCTL_EN) && e1000_has_rxbufs(s, 1);
}