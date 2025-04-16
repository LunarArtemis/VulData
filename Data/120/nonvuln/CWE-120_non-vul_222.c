static int
ismt_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	int err;
	struct ismt_priv *priv;
	unsigned long start, len;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	pci_set_drvdata(pdev, priv);

	i2c_set_adapdata(&priv->adapter, priv);
	priv->adapter.owner = THIS_MODULE;
	priv->adapter.class = I2C_CLASS_HWMON;
	priv->adapter.algo = &smbus_algorithm;
	priv->adapter.dev.parent = &pdev->dev;
	ACPI_COMPANION_SET(&priv->adapter.dev, ACPI_COMPANION(&pdev->dev));
	priv->adapter.retries = ISMT_MAX_RETRIES;

	priv->pci_dev = pdev;

	err = pcim_enable_device(pdev);
	if (err) {
		dev_err(&pdev->dev, "Failed to enable SMBus PCI device (%d)\n",
			err);
		return err;
	}

	/* enable bus mastering */
	pci_set_master(pdev);

	/* Determine the address of the SMBus area */
	start = pci_resource_start(pdev, SMBBAR);
	len = pci_resource_len(pdev, SMBBAR);
	if (!start || !len) {
		dev_err(&pdev->dev,
			"SMBus base address uninitialized, upgrade BIOS\n");
		return -ENODEV;
	}

	snprintf(priv->adapter.name, sizeof(priv->adapter.name),
		 "SMBus iSMT adapter at %lx", start);

	dev_dbg(&priv->pci_dev->dev, " start=0x%lX\n", start);
	dev_dbg(&priv->pci_dev->dev, " len=0x%lX\n", len);

	err = acpi_check_resource_conflict(&pdev->resource[SMBBAR]);
	if (err) {
		dev_err(&pdev->dev, "ACPI resource conflict!\n");
		return err;
	}

	err = pci_request_region(pdev, SMBBAR, ismt_driver.name);
	if (err) {
		dev_err(&pdev->dev,
			"Failed to request SMBus region 0x%lx-0x%lx\n",
			start, start + len);
		return err;
	}

	priv->smba = pcim_iomap(pdev, SMBBAR, len);
	if (!priv->smba) {
		dev_err(&pdev->dev, "Unable to ioremap SMBus BAR\n");
		return -ENODEV;
	}

	err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));
	if (err) {
		err = dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(32));
		if (err) {
			dev_err(&pdev->dev, "dma_set_mask fail\n");
			return -ENODEV;
		}
	}

	err = ismt_dev_init(priv);
	if (err)
		return err;

	ismt_hw_init(priv);

	err = ismt_int_init(priv);
	if (err)
		return err;

	err = i2c_add_adapter(&priv->adapter);
	if (err)
		return -ENODEV;
	return 0;
}