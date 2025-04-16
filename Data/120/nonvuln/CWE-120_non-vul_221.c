static irqreturn_t ismt_handle_isr(struct ismt_priv *priv)
{
	complete(&priv->cmp);

	return IRQ_HANDLED;
}