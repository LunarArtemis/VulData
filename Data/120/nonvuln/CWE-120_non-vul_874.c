static int stellaris_enet_can_receive(stellaris_enet_state *s)
{
    return (s->np < 31);
}