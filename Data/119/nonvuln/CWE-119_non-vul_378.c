static int cqspi_command_write(struct spi_nor *nor, const u8 opcode,
			       const u8 *txbuf, const unsigned n_tx)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *reg_base = cqspi->iobase;
	unsigned int reg;
	unsigned int data;
	int ret;

	if (n_tx > 4 || (n_tx && !txbuf)) {
		dev_err(nor->dev,
			"Invalid input argument, cmdlen %d txbuf 0x%p\n",
			n_tx, txbuf);
		return -EINVAL;
	}

	reg = opcode << CQSPI_REG_CMDCTRL_OPCODE_LSB;
	if (n_tx) {
		reg |= (0x1 << CQSPI_REG_CMDCTRL_WR_EN_LSB);
		reg |= ((n_tx - 1) & CQSPI_REG_CMDCTRL_WR_BYTES_MASK)
			<< CQSPI_REG_CMDCTRL_WR_BYTES_LSB;
		data = 0;
		memcpy(&data, txbuf, n_tx);
		writel(data, reg_base + CQSPI_REG_CMDWRITEDATALOWER);
	}

	ret = cqspi_exec_flash_cmd(cqspi, reg);
	return ret;
}