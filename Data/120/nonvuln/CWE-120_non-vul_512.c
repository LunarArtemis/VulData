static void qdev_e1000_reset(DeviceState *dev)
{
    E1000State *d = DO_UPCAST(E1000State, dev.qdev, dev);
    e1000_reset(d);
}