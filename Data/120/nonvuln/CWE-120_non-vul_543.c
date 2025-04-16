static void e1000_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->init = pci_e1000_init;
    k->exit = pci_e1000_uninit;
    k->romfile = "pxe-e1000.rom";
    k->vendor_id = PCI_VENDOR_ID_INTEL;
    k->device_id = E1000_DEVID;
    k->revision = 0x03;
    k->class_id = PCI_CLASS_NETWORK_ETHERNET;
    dc->desc = "Intel Gigabit Ethernet";
    dc->reset = qdev_e1000_reset;
    dc->vmsd = &vmstate_e1000;
    dc->props = e1000_properties;
}