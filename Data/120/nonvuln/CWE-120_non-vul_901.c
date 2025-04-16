static int si4713_tx_tune_status(struct si4713_device *sdev, u8 intack,
					u16 *frequency,	u8 *power,
					u8 *antcap, u8 *noise)
{
	int err;
	u8 val[SI4713_TXSTATUS_NRESP];
	/*
	 * 	.First byte = intack bit
	 */
	const u8 args[SI4713_TXSTATUS_NARGS] = {
		intack & SI4713_INTACK_MASK,
	};

	err = si4713_send_command(sdev, SI4713_CMD_TX_TUNE_STATUS,
				  args, ARRAY_SIZE(args), val,
				  ARRAY_SIZE(val), DEFAULT_TIMEOUT);

	if (!err) {
		v4l2_dbg(1, debug, &sdev->sd,
			"%s: status=0x%02x\n", __func__, val[0]);
		*frequency = compose_u16(val[2], val[3]);
		sdev->frequency = *frequency;
		*power = val[5];
		*antcap = val[6];
		*noise = val[7];
		v4l2_dbg(1, debug, &sdev->sd, "%s: response: %d x 10 kHz "
				"(power %d, antcap %d, rnl %d)\n", __func__,
				*frequency, *power, *antcap, *noise);
	}

	return err;
}