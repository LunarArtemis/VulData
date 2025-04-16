static int e1000_post_load(void *opaque, int version_id)
{
    E1000State *s = opaque;

    /* nc.link_down can't be migrated, so infer link_down according
     * to link status bit in mac_reg[STATUS] */
    s->nic->nc.link_down = (s->mac_reg[STATUS] & E1000_STATUS_LU) == 0;

    return 0;
}