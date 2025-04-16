static void cqspi_configure_cs_and_sizes(struct spi_nor *nor)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	void __iomem *iobase = cqspi->iobase;
	unsigned int reg;

	/* configure page size and block size. */
	reg = readl(iobase + CQSPI_REG_SIZE);
	reg &= ~(CQSPI_REG_SIZE_PAGE_MASK << CQSPI_REG_SIZE_PAGE_LSB);
	reg &= ~(CQSPI_REG_SIZE_BLOCK_MASK << CQSPI_REG_SIZE_BLOCK_LSB);
	reg &= ~CQSPI_REG_SIZE_ADDRESS_MASK;
	reg |= (nor->page_size << CQSPI_REG_SIZE_PAGE_LSB);
	reg |= (ilog2(nor->mtd.erasesize) << CQSPI_REG_SIZE_BLOCK_LSB);
	reg |= (nor->addr_width - 1);
	writel(reg, iobase + CQSPI_REG_SIZE);

	/* configure the chip select */
	cqspi_chipselect(nor);

	/* Store the new configuration of the controller */
	cqspi->current_page_size = nor->page_size;
	cqspi->current_erase_size = nor->mtd.erasesize;
	cqspi->current_addr_width = nor->addr_width;
}