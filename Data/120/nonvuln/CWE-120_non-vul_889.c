static int si4713_powerup(struct si4713_device *sdev)
{
	int err;
	u8 resp[SI4713_PWUP_NRESP];
	/*
	 * 	.First byte = Enabled interrupts and boot function
	 * 	.Second byte = Input operation mode
	 */
	const u8 args[SI4713_PWUP_NARGS] = {
		SI4713_PWUP_CTSIEN | SI4713_PWUP_GPO2OEN | SI4713_PWUP_FUNC_TX,
		SI4713_PWUP_OPMOD_ANALOG,
	};

	if (sdev->power_state)
		return 0;

	err = regulator_bulk_enable(ARRAY_SIZE(sdev->supplies),
				    sdev->supplies);
	if (err) {
		v4l2_err(&sdev->sd, "Failed to enable supplies: %d\n", err);
		return err;
	}
	if (gpio_is_valid(sdev->gpio_reset)) {
		udelay(50);
		gpio_set_value(sdev->gpio_reset, 1);
	}

	err = si4713_send_command(sdev, SI4713_CMD_POWER_UP,
					args, ARRAY_SIZE(args),
					resp, ARRAY_SIZE(resp),
					TIMEOUT_POWER_UP);

	if (!err) {
		v4l2_dbg(1, debug, &sdev->sd, "Powerup response: 0x%02x\n",
				resp[0]);
		v4l2_dbg(1, debug, &sdev->sd, "Device in power up mode\n");
		sdev->power_state = POWER_ON;

		err = si4713_write_property(sdev, SI4713_GPO_IEN,
						SI4713_STC_INT | SI4713_CTS);
	} else {
		if (gpio_is_valid(sdev->gpio_reset))
			gpio_set_value(sdev->gpio_reset, 0);
		err = regulator_bulk_disable(ARRAY_SIZE(sdev->supplies),
					     sdev->supplies);
		if (err)
			v4l2_err(&sdev->sd,
				 "Failed to disable supplies: %d\n", err);
	}

	return err;
}