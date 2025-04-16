static int ismt_int_init(struct ismt_priv *priv)
{
	int err;

	/* Try using MSI interrupts */
	err = pci_enable_msi(priv->pci_dev);
	if (err)
		goto intx;

	err = devm_request_irq(&priv->pci_dev->dev,
			       priv->pci_dev->irq,
			       ismt_do_msi_interrupt,
			       0,
			       "ismt-msi",
			       priv);
	if (err) {
		pci_disable_msi(priv->pci_dev);
		goto intx;
	}

	return 0;

	/* Try using legacy interrupts */
intx:
	dev_warn(&priv->pci_dev->dev,
		 "Unable to use MSI interrupts, falling back to legacy\n");

	err = devm_request_irq(&priv->pci_dev->dev,
			       priv->pci_dev->irq,
			       ismt_do_interrupt,
			       IRQF_SHARED,
			       "ismt-intx",
			       priv);
	if (err) {
		dev_err(&priv->pci_dev->dev, "no usable interrupts\n");
		return err;
	}

	return 0;
}