static int si4713_write_property(struct si4713_device *sdev, u16 prop, u16 val)
{
	int rval;
	u8 resp[SI4713_SET_PROP_NRESP];
	/*
	 * 	.First byte = 0
	 * 	.Second byte = property's MSB
	 * 	.Third byte = property's LSB
	 * 	.Fourth byte = value's MSB
	 * 	.Fifth byte = value's LSB
	 */
	const u8 args[SI4713_SET_PROP_NARGS] = {
		0x00,
		msb(prop),
		lsb(prop),
		msb(val),
		lsb(val),
	};

	rval = si4713_send_command(sdev, SI4713_CMD_SET_PROPERTY,
					args, ARRAY_SIZE(args),
					resp, ARRAY_SIZE(resp),
					DEFAULT_TIMEOUT);

	if (rval < 0)
		return rval;

	v4l2_dbg(1, debug, &sdev->sd,
			"%s: property=0x%02x value=0x%02x status=0x%02x\n",
			__func__, prop, val, resp[0]);

	/*
	 * As there is no command response for SET_PROPERTY,
	 * wait Tcomp time to finish before proceed, in order
	 * to have property properly set.
	 */
	msleep(TIMEOUT_SET_PROPERTY);

	return rval;
}