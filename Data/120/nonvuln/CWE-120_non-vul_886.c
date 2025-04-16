static int si4713_read_property(struct si4713_device *sdev, u16 prop, u32 *pv)
{
	int err;
	u8 val[SI4713_GET_PROP_NRESP];
	/*
	 * 	.First byte = 0
	 * 	.Second byte = property's MSB
	 * 	.Third byte = property's LSB
	 */
	const u8 args[SI4713_GET_PROP_NARGS] = {
		0x00,
		msb(prop),
		lsb(prop),
	};

	err = si4713_send_command(sdev, SI4713_CMD_GET_PROPERTY,
				  args, ARRAY_SIZE(args), val,
				  ARRAY_SIZE(val), DEFAULT_TIMEOUT);

	if (err < 0)
		return err;

	*pv = compose_u16(val[2], val[3]);

	v4l2_dbg(1, debug, &sdev->sd,
			"%s: property=0x%02x value=0x%02x status=0x%02x\n",
			__func__, prop, *pv, val[0]);

	return err;
}