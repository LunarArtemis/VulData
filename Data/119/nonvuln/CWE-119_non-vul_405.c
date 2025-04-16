static int cqspi_indirect_read_setup(struct spi_nor *nor,
				     const unsigned int from_addr)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *reg_base = cqspi->iobase;
	unsigned int dummy_clk = 0;
	unsigned int reg;

	writel(from_addr, reg_base + CQSPI_REG_INDIRECTRDSTARTADDR);

	reg = nor->read_opcode << CQSPI_REG_RD_INSTR_OPCODE_LSB;
	reg |= cqspi_calc_rdreg(nor, nor->read_opcode);

	/* Setup dummy clock cycles */
	dummy_clk = nor->read_dummy;
	if (dummy_clk > CQSPI_DUMMY_CLKS_MAX)
		dummy_clk = CQSPI_DUMMY_CLKS_MAX;

	if (dummy_clk / 8) {
		reg |= (1 << CQSPI_REG_RD_INSTR_MODE_EN_LSB);
		/* Set mode bits high to ensure chip doesn't enter XIP */
		writel(0xFF, reg_base + CQSPI_REG_MODE_BIT);

		/* Need to subtract the mode byte (8 clocks). */
		if (f_pdata->inst_width != CQSPI_INST_TYPE_QUAD)
			dummy_clk -= 8;

		if (dummy_clk)
			reg |= (dummy_clk & CQSPI_REG_RD_INSTR_DUMMY_MASK)
			       << CQSPI_REG_RD_INSTR_DUMMY_LSB;
	}

	writel(reg, reg_base + CQSPI_REG_RD_INSTR);

	/* Set address width */
	reg = readl(reg_base + CQSPI_REG_SIZE);
	reg &= ~CQSPI_REG_SIZE_ADDRESS_MASK;
	reg |= (nor->addr_width - 1);
	writel(reg, reg_base + CQSPI_REG_SIZE);
	return 0;
}