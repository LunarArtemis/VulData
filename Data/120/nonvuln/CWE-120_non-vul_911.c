static int si4713_probe(struct i2c_client *client,
					const struct i2c_device_id *id)
{
	struct si4713_device *sdev;
	struct si4713_platform_data *pdata = client->dev.platform_data;
	int rval, i;

	sdev = kzalloc(sizeof *sdev, GFP_KERNEL);
	if (!sdev) {
		dev_err(&client->dev, "Failed to alloc video device.\n");
		rval = -ENOMEM;
		goto exit;
	}

	sdev->gpio_reset = -1;
	if (pdata && gpio_is_valid(pdata->gpio_reset)) {
		rval = gpio_request(pdata->gpio_reset, "si4713 reset");
		if (rval) {
			dev_err(&client->dev,
				"Failed to request gpio: %d\n", rval);
			goto free_sdev;
		}
		sdev->gpio_reset = pdata->gpio_reset;
		gpio_direction_output(sdev->gpio_reset, 0);
	}

	for (i = 0; i < ARRAY_SIZE(sdev->supplies); i++)
		sdev->supplies[i].supply = si4713_supply_names[i];

	rval = regulator_bulk_get(&client->dev, ARRAY_SIZE(sdev->supplies),
				  sdev->supplies);
	if (rval) {
		dev_err(&client->dev, "Cannot get regulators: %d\n", rval);
		goto free_gpio;
	}

	v4l2_i2c_subdev_init(&sdev->sd, client, &si4713_subdev_ops);

	mutex_init(&sdev->mutex);
	init_completion(&sdev->work);

	if (client->irq) {
		rval = request_irq(client->irq,
			si4713_handler, IRQF_TRIGGER_FALLING | IRQF_DISABLED,
			client->name, sdev);
		if (rval < 0) {
			v4l2_err(&sdev->sd, "Could not request IRQ\n");
			goto put_reg;
		}
		v4l2_dbg(1, debug, &sdev->sd, "IRQ requested.\n");
	} else {
		v4l2_warn(&sdev->sd, "IRQ not configured. Using timeouts.\n");
	}

	rval = si4713_initialize(sdev);
	if (rval < 0) {
		v4l2_err(&sdev->sd, "Failed to probe device information.\n");
		goto free_irq;
	}

	return 0;

free_irq:
	if (client->irq)
		free_irq(client->irq, sdev);
put_reg:
	regulator_bulk_free(ARRAY_SIZE(sdev->supplies), sdev->supplies);
free_gpio:
	if (gpio_is_valid(sdev->gpio_reset))
		gpio_free(sdev->gpio_reset);
free_sdev:
	kfree(sdev);
exit:
	return rval;
}