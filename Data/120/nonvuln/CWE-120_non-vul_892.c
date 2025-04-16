static int si4713_tx_tune_freq(struct si4713_device *sdev, u16 frequency)
{
	int err;
	u8 val[SI4713_TXFREQ_NRESP];
	/*
	 * 	.First byte = 0
	 * 	.Second byte = frequency's MSB
	 * 	.Third byte = frequency's LSB
	 */
	const u8 args[SI4713_TXFREQ_NARGS] = {
		0x00,
		msb(frequency),
		lsb(frequency),
	};

	err = si4713_send_command(sdev, SI4713_CMD_TX_TUNE_FREQ,
				  args, ARRAY_SIZE(args), val,
				  ARRAY_SIZE(val), DEFAULT_TIMEOUT);

	if (err < 0)
		return err;

	v4l2_dbg(1, debug, &sdev->sd,
			"%s: frequency=0x%02x status=0x%02x\n", __func__,
			frequency, val[0]);

	err = si4713_wait_stc(sdev, TIMEOUT_TX_TUNE);
	if (err < 0)
		return err;

	return compose_u16(args[1], args[2]);
}