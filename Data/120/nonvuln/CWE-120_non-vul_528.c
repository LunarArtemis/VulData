static void
pci_e1000_uninit(PCIDevice *dev)
{
    E1000State *d = DO_UPCAST(E1000State, dev, dev);

    qemu_del_timer(d->autoneg_timer);
    qemu_free_timer(d->autoneg_timer);
    memory_region_destroy(&d->mmio);
    memory_region_destroy(&d->io);
    qemu_del_net_client(&d->nic->nc);
}