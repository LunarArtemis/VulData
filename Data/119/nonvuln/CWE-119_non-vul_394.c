static void cqspi_unprep(struct spi_nor *nor, enum spi_nor_ops ops)
{
	struct cqspi_flash_pdata *f_pdata = nor->priv;
	struct cqspi_st *cqspi = f_pdata->cqspi;

	mutex_unlock(&cqspi->bus_mutex);
}