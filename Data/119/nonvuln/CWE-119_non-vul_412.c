static void cqspi_configure(struct spi_nor *nor)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;
	const unsigned int sclk = f_pdata->clk_rate;
	int switch_cs = (cqspi->current_cs != f_pdata->cs);
	int switch_ck = (cqspi->sclk != sclk);

	if ((cqspi->current_page_size != nor->page_size) ||
	    (cqspi->current_erase_size != nor->mtd.erasesize) ||
	    (cqspi->current_addr_width != nor->addr_width))
		switch_cs = 1;

	if (switch_cs || switch_ck)
		cqspi_controller_enable(cqspi, 0);

	/* Switch chip select. */
	if (switch_cs) {
		cqspi->current_cs = f_pdata->cs;
		cqspi_configure_cs_and_sizes(nor);
	}

	/* Setup baudrate divisor and delays */
	if (switch_ck) {
		cqspi->sclk = sclk;
		cqspi_config_baudrate_div(cqspi);
		cqspi_delay(nor);
		cqspi_readdata_capture(cqspi, 1, f_pdata->read_delay);
	}

	if (switch_cs || switch_ck)
		cqspi_controller_enable(cqspi, 1);
}