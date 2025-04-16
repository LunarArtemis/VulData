static int cqspi_remove(struct platform_device *pdev)
{
	struct cqspi_st *cqspi = platform_get_drvdata(pdev);
	int i;

	for (i = 0; i < CQSPI_MAX_CHIPSELECT; i++)
		if (cqspi->f_pdata[i].registered)
			mtd_device_unregister(&cqspi->f_pdata[i].nor.mtd);

	cqspi_controller_enable(cqspi, 0);

	clk_disable_unprepare(cqspi->clk);

	return 0;
}