static void ismt_submit_desc(struct ismt_priv *priv)
{
	uint fmhp;
	uint val;

	ismt_desc_dump(priv);
	ismt_gen_reg_dump(priv);
	ismt_mstr_reg_dump(priv);

	/* Set the FMHP (Firmware Master Head Pointer)*/
	fmhp = ((priv->head + 1) % ISMT_DESC_ENTRIES) << 16;
	val = readl(priv->smba + ISMT_MSTR_MCTRL);
	writel((val & ~ISMT_MCTRL_FMHP) | fmhp,
	       priv->smba + ISMT_MSTR_MCTRL);

	/* Set the start bit */
	val = readl(priv->smba + ISMT_MSTR_MCTRL);
	writel(val | ISMT_MCTRL_SS,
	       priv->smba + ISMT_MSTR_MCTRL);
}