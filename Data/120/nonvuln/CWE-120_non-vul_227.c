static void ismt_desc_dump(struct ismt_priv *priv)
{
	struct device *dev = &priv->pci_dev->dev;
	struct ismt_desc *desc = &priv->hw[priv->head];

	dev_dbg(dev, "Dump of the descriptor struct:  0x%X\n", priv->head);
	__ismt_desc_dump(dev, desc);
}