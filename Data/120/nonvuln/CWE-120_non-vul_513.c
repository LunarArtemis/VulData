static void
e1000_set_link_status(NetClientState *nc)
{
    E1000State *s = DO_UPCAST(NICState, nc, nc)->opaque;
    uint32_t old_status = s->mac_reg[STATUS];

    if (nc->link_down) {
        e1000_link_down(s);
    } else {
        e1000_link_up(s);
    }

    if (s->mac_reg[STATUS] != old_status)
        set_ics(s, 0, E1000_ICR_LSC);
}