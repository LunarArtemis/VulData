static int si4713_tx_tune_power(struct si4713_device *sdev, u8 power,
				u8 antcap)
{
	int err;
	u8 val[SI4713_TXPWR_NRESP];
	/*
	 * 	.First byte = 0
	 * 	.Second byte = 0
	 * 	.Third byte = power
	 * 	.Fourth byte = antcap
	 */
	const u8 args[SI4713_TXPWR_NARGS] = {
		0x00,
		0x00,
		power,
		antcap,
	};

	if (((power > 0) && (power < SI4713_MIN_POWER)) ||
		power > SI4713_MAX_POWER || antcap > SI4713_MAX_ANTCAP)
		return -EDOM;

	err = si4713_send_command(sdev, SI4713_CMD_TX_TUNE_POWER,
				  args, ARRAY_SIZE(args), val,
				  ARRAY_SIZE(val), DEFAULT_TIMEOUT);

	if (err < 0)
		return err;

	v4l2_dbg(1, debug, &sdev->sd,
			"%s: power=0x%02x antcap=0x%02x status=0x%02x\n",
			__func__, power, antcap, val[0]);

	return si4713_wait_stc(sdev, TIMEOUT_TX_TUNE_POWER);
}