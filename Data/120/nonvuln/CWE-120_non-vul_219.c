static int ismt_dev_init(struct ismt_priv *priv)
{
	/* allocate memory for the descriptor */
	priv->hw = dmam_alloc_coherent(&priv->pci_dev->dev,
				       (ISMT_DESC_ENTRIES
					       * sizeof(struct ismt_desc)),
				       &priv->io_rng_dma,
				       GFP_KERNEL);
	if (!priv->hw)
		return -ENOMEM;

	priv->head = 0;
	init_completion(&priv->cmp);

	priv->log = dmam_alloc_coherent(&priv->pci_dev->dev,
					ISMT_LOG_ENTRIES * sizeof(u32),
					&priv->log_dma, GFP_KERNEL);
	if (!priv->log)
		return -ENOMEM;

	return 0;
}