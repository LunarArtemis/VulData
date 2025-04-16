static int si4713_tx_tune_measure(struct si4713_device *sdev, u16 frequency,
					u8 antcap)
{
	int err;
	u8 val[SI4713_TXMEA_NRESP];
	/*
	 * 	.First byte = 0
	 * 	.Second byte = frequency's MSB
	 * 	.Third byte = frequency's LSB
	 * 	.Fourth byte = antcap
	 */
	const u8 args[SI4713_TXMEA_NARGS] = {
		0x00,
		msb(frequency),
		lsb(frequency),
		antcap,
	};

	sdev->tune_rnl = DEFAULT_TUNE_RNL;

	if (antcap > SI4713_MAX_ANTCAP)
		return -EDOM;

	err = si4713_send_command(sdev, SI4713_CMD_TX_TUNE_MEASURE,
				  args, ARRAY_SIZE(args), val,
				  ARRAY_SIZE(val), DEFAULT_TIMEOUT);

	if (err < 0)
		return err;

	v4l2_dbg(1, debug, &sdev->sd,
			"%s: frequency=0x%02x antcap=0x%02x status=0x%02x\n",
			__func__, frequency, antcap, val[0]);

	return si4713_wait_stc(sdev, TIMEOUT_TX_TUNE);
}