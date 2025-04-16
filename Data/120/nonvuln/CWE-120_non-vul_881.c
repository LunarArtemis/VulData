static int si4713_powerdown(struct si4713_device *sdev)
{
	int err;
	u8 resp[SI4713_PWDN_NRESP];

	if (!sdev->power_state)
		return 0;

	err = si4713_send_command(sdev, SI4713_CMD_POWER_DOWN,
					NULL, 0,
					resp, ARRAY_SIZE(resp),
					DEFAULT_TIMEOUT);

	if (!err) {
		v4l2_dbg(1, debug, &sdev->sd, "Power down response: 0x%02x\n",
				resp[0]);
		v4l2_dbg(1, debug, &sdev->sd, "Device in reset mode\n");
		if (gpio_is_valid(sdev->gpio_reset))
			gpio_set_value(sdev->gpio_reset, 0);
		err = regulator_bulk_disable(ARRAY_SIZE(sdev->supplies),
					     sdev->supplies);
		if (err)
			v4l2_err(&sdev->sd,
				 "Failed to disable supplies: %d\n", err);
		sdev->power_state = POWER_OFF;
	}

	return err;
}