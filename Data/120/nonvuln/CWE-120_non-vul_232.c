static void ismt_remove(struct pci_dev *pdev)
{
	struct ismt_priv *priv = pci_get_drvdata(pdev);

	i2c_del_adapter(&priv->adapter);
}