static irqreturn_t ismt_do_interrupt(int vec, void *data)
{
	u32 val;
	struct ismt_priv *priv = data;

	/*
	 * check to see it's our interrupt, return IRQ_NONE if not ours
	 * since we are sharing interrupt
	 */
	val = readl(priv->smba + ISMT_MSTR_MSTS);

	if (!(val & (ISMT_MSTS_MIS | ISMT_MSTS_MEIS)))
		return IRQ_NONE;
	else
		writel(val | ISMT_MSTS_MIS | ISMT_MSTS_MEIS,
		       priv->smba + ISMT_MSTR_MSTS);

	return ismt_handle_isr(priv);
}