static void
e1000_cleanup(NetClientState *nc)
{
    E1000State *s = DO_UPCAST(NICState, nc, nc)->opaque;

    s->nic = NULL;
}