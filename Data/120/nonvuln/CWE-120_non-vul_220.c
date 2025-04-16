static void ismt_gen_reg_dump(struct ismt_priv *priv)
{
	struct device *dev = &priv->pci_dev->dev;

	dev_dbg(dev, "Dump of the iSMT General Registers\n");
	dev_dbg(dev, "  GCTRL.... : (0x%p)=0x%X\n",
		priv->smba + ISMT_GR_GCTRL,
		readl(priv->smba + ISMT_GR_GCTRL));
	dev_dbg(dev, "  SMTICL... : (0x%p)=0x%016llX\n",
		priv->smba + ISMT_GR_SMTICL,
		(long long unsigned int)readq(priv->smba + ISMT_GR_SMTICL));
	dev_dbg(dev, "  ERRINTMSK : (0x%p)=0x%X\n",
		priv->smba + ISMT_GR_ERRINTMSK,
		readl(priv->smba + ISMT_GR_ERRINTMSK));
	dev_dbg(dev, "  ERRAERMSK : (0x%p)=0x%X\n",
		priv->smba + ISMT_GR_ERRAERMSK,
		readl(priv->smba + ISMT_GR_ERRAERMSK));
	dev_dbg(dev, "  ERRSTS... : (0x%p)=0x%X\n",
		priv->smba + ISMT_GR_ERRSTS,
		readl(priv->smba + ISMT_GR_ERRSTS));
	dev_dbg(dev, "  ERRINFO.. : (0x%p)=0x%X\n",
		priv->smba + ISMT_GR_ERRINFO,
		readl(priv->smba + ISMT_GR_ERRINFO));
}