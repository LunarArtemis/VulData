static void ismt_mstr_reg_dump(struct ismt_priv *priv)
{
	struct device *dev = &priv->pci_dev->dev;

	dev_dbg(dev, "Dump of the iSMT Master Registers\n");
	dev_dbg(dev, "  MDBA..... : (0x%p)=0x%016llX\n",
		priv->smba + ISMT_MSTR_MDBA,
		(long long unsigned int)readq(priv->smba + ISMT_MSTR_MDBA));
	dev_dbg(dev, "  MCTRL.... : (0x%p)=0x%X\n",
		priv->smba + ISMT_MSTR_MCTRL,
		readl(priv->smba + ISMT_MSTR_MCTRL));
	dev_dbg(dev, "  MSTS..... : (0x%p)=0x%X\n",
		priv->smba + ISMT_MSTR_MSTS,
		readl(priv->smba + ISMT_MSTR_MSTS));
	dev_dbg(dev, "  MDS...... : (0x%p)=0x%X\n",
		priv->smba + ISMT_MSTR_MDS,
		readl(priv->smba + ISMT_MSTR_MDS));
	dev_dbg(dev, "  RPOLICY.. : (0x%p)=0x%X\n",
		priv->smba + ISMT_MSTR_RPOLICY,
		readl(priv->smba + ISMT_MSTR_RPOLICY));
	dev_dbg(dev, "  SPGT..... : (0x%p)=0x%X\n",
		priv->smba + ISMT_SPGT,
		readl(priv->smba + ISMT_SPGT));
}