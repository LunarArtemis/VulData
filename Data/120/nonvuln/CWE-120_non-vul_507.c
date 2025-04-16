static uint64_t e1000_io_read(void *opaque, hwaddr addr,
                              unsigned size)
{
    E1000State *s = opaque;

    (void)s;
    return 0;
}