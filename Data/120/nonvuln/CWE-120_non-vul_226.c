static void ismt_hw_init(struct ismt_priv *priv)
{
	u32 val;
	struct device *dev = &priv->pci_dev->dev;

	/* initialize the Master Descriptor Base Address (MDBA) */
	writeq(priv->io_rng_dma, priv->smba + ISMT_MSTR_MDBA);

	writeq(priv->log_dma, priv->smba + ISMT_GR_SMTICL);

	/* initialize the Master Control Register (MCTRL) */
	writel(ISMT_MCTRL_MEIE, priv->smba + ISMT_MSTR_MCTRL);

	/* initialize the Master Status Register (MSTS) */
	writel(0, priv->smba + ISMT_MSTR_MSTS);

	/* initialize the Master Descriptor Size (MDS) */
	val = readl(priv->smba + ISMT_MSTR_MDS);
	writel((val & ~ISMT_MDS_MASK) | (ISMT_DESC_ENTRIES - 1),
		priv->smba + ISMT_MSTR_MDS);

	/*
	 * Set the SMBus speed (could use this for slow HW debuggers)
	 */

	val = readl(priv->smba + ISMT_SPGT);

	switch (bus_speed) {
	case 0:
		break;

	case 80:
		dev_dbg(dev, "Setting SMBus clock to 80 kHz\n");
		writel(((val & ~ISMT_SPGT_SPD_MASK) | ISMT_SPGT_SPD_80K),
			priv->smba + ISMT_SPGT);
		break;

	case 100:
		dev_dbg(dev, "Setting SMBus clock to 100 kHz\n");
		writel(((val & ~ISMT_SPGT_SPD_MASK) | ISMT_SPGT_SPD_100K),
			priv->smba + ISMT_SPGT);
		break;

	case 400:
		dev_dbg(dev, "Setting SMBus clock to 400 kHz\n");
		writel(((val & ~ISMT_SPGT_SPD_MASK) | ISMT_SPGT_SPD_400K),
			priv->smba + ISMT_SPGT);
		break;

	case 1000:
		dev_dbg(dev, "Setting SMBus clock to 1000 kHz\n");
		writel(((val & ~ISMT_SPGT_SPD_MASK) | ISMT_SPGT_SPD_1M),
			priv->smba + ISMT_SPGT);
		break;

	default:
		dev_warn(dev, "Invalid SMBus clock speed, only 0, 80, 100, 400, and 1000 are valid\n");
		break;
	}

	val = readl(priv->smba + ISMT_SPGT);

	switch (val & ISMT_SPGT_SPD_MASK) {
	case ISMT_SPGT_SPD_80K:
		bus_speed = 80;
		break;
	case ISMT_SPGT_SPD_100K:
		bus_speed = 100;
		break;
	case ISMT_SPGT_SPD_400K:
		bus_speed = 400;
		break;
	case ISMT_SPGT_SPD_1M:
		bus_speed = 1000;
		break;
	}
	dev_dbg(dev, "SMBus clock is running at %d kHz\n", bus_speed);
}