static int pci_e1000_init(PCIDevice *pci_dev)
{
    E1000State *d = DO_UPCAST(E1000State, dev, pci_dev);
    uint8_t *pci_conf;
    uint16_t checksum = 0;
    int i;
    uint8_t *macaddr;

    pci_conf = d->dev.config;

    /* TODO: RST# value should be 0, PCI spec 6.2.4 */
    pci_conf[PCI_CACHE_LINE_SIZE] = 0x10;

    pci_conf[PCI_INTERRUPT_PIN] = 1; /* interrupt pin A */

    e1000_mmio_setup(d);

    pci_register_bar(&d->dev, 0, PCI_BASE_ADDRESS_SPACE_MEMORY, &d->mmio);

    pci_register_bar(&d->dev, 1, PCI_BASE_ADDRESS_SPACE_IO, &d->io);

    memmove(d->eeprom_data, e1000_eeprom_template,
        sizeof e1000_eeprom_template);
    qemu_macaddr_default_if_unset(&d->conf.macaddr);
    macaddr = d->conf.macaddr.a;
    for (i = 0; i < 3; i++)
        d->eeprom_data[i] = (macaddr[2*i+1]<<8) | macaddr[2*i];
    for (i = 0; i < EEPROM_CHECKSUM_REG; i++)
        checksum += d->eeprom_data[i];
    checksum = (uint16_t) EEPROM_SUM - checksum;
    d->eeprom_data[EEPROM_CHECKSUM_REG] = checksum;

    d->nic = qemu_new_nic(&net_e1000_info, &d->conf,
                          object_get_typename(OBJECT(d)), d->dev.qdev.id, d);

    qemu_format_nic_info_str(&d->nic->nc, macaddr);

    add_boot_device_path(d->conf.bootindex, &pci_dev->qdev, "/ethernet-phy@0");

    d->autoneg_timer = qemu_new_timer_ms(vm_clock, e1000_autoneg_timer, d);

    return 0;
}