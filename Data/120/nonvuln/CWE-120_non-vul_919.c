static int si4713_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct si4713_device *sdev = to_si4713_device(sd);

	if (sdev->power_state)
		si4713_set_power_state(sdev, POWER_DOWN);

	if (client->irq > 0)
		free_irq(client->irq, sdev);

	v4l2_device_unregister_subdev(sd);
	regulator_bulk_free(ARRAY_SIZE(sdev->supplies), sdev->supplies);
	if (gpio_is_valid(sdev->gpio_reset))
		gpio_free(sdev->gpio_reset);
	kfree(sdev);

	return 0;
}