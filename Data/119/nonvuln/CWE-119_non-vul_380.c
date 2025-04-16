static int cqspi_set_protocol(struct spi_nor *nor, const int read)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;

	f_pdata->inst_width = CQSPI_INST_TYPE_SINGLE;
	f_pdata->addr_width = CQSPI_INST_TYPE_SINGLE;
	f_pdata->data_width = CQSPI_INST_TYPE_SINGLE;

	if (read) {
		switch (nor->flash_read) {
		case SPI_NOR_NORMAL:
		case SPI_NOR_FAST:
			f_pdata->data_width = CQSPI_INST_TYPE_SINGLE;
			break;
		case SPI_NOR_DUAL:
			f_pdata->data_width = CQSPI_INST_TYPE_DUAL;
			break;
		case SPI_NOR_QUAD:
			f_pdata->data_width = CQSPI_INST_TYPE_QUAD;
			break;
		default:
			return -EINVAL;
		}
	}

	cqspi_configure(nor);

	return 0;
}