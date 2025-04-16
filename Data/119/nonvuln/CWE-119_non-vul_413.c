static int cqspi_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct device *dev = &pdev->dev;
	struct cqspi_st *cqspi;
	struct resource *res;
	struct resource *res_ahb;
	int ret;
	int irq;

	cqspi = devm_kzalloc(dev, sizeof(*cqspi), GFP_KERNEL);
	if (!cqspi)
		return -ENOMEM;

	mutex_init(&cqspi->bus_mutex);
	cqspi->pdev = pdev;
	platform_set_drvdata(pdev, cqspi);

	/* Obtain configuration from OF. */
	ret = cqspi_of_get_pdata(pdev);
	if (ret) {
		dev_err(dev, "Cannot get mandatory OF data.\n");
		return -ENODEV;
	}

	/* Obtain QSPI clock. */
	cqspi->clk = devm_clk_get(dev, NULL);
	if (IS_ERR(cqspi->clk)) {
		dev_err(dev, "Cannot claim QSPI clock.\n");
		return PTR_ERR(cqspi->clk);
	}

	/* Obtain and remap controller address. */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	cqspi->iobase = devm_ioremap_resource(dev, res);
	if (IS_ERR(cqspi->iobase)) {
		dev_err(dev, "Cannot remap controller address.\n");
		return PTR_ERR(cqspi->iobase);
	}

	/* Obtain and remap AHB address. */
	res_ahb = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	cqspi->ahb_base = devm_ioremap_resource(dev, res_ahb);
	if (IS_ERR(cqspi->ahb_base)) {
		dev_err(dev, "Cannot remap AHB address.\n");
		return PTR_ERR(cqspi->ahb_base);
	}

	init_completion(&cqspi->transfer_complete);

	/* Obtain IRQ line. */
	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(dev, "Cannot obtain IRQ.\n");
		return -ENXIO;
	}

	ret = clk_prepare_enable(cqspi->clk);
	if (ret) {
		dev_err(dev, "Cannot enable QSPI clock.\n");
		return ret;
	}

	cqspi->master_ref_clk_hz = clk_get_rate(cqspi->clk);

	ret = devm_request_irq(dev, irq, cqspi_irq_handler, 0,
			       pdev->name, cqspi);
	if (ret) {
		dev_err(dev, "Cannot request IRQ.\n");
		goto probe_irq_failed;
	}

	cqspi_wait_idle(cqspi);
	cqspi_controller_init(cqspi);
	cqspi->current_cs = -1;
	cqspi->sclk = 0;

	ret = cqspi_setup_flash(cqspi, np);
	if (ret) {
		dev_err(dev, "Cadence QSPI NOR probe failed %d\n", ret);
		goto probe_setup_failed;
	}

	return ret;
probe_irq_failed:
	cqspi_controller_enable(cqspi, 0);
probe_setup_failed:
	clk_disable_unprepare(cqspi->clk);
	return ret;
}