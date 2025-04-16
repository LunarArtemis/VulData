static int si4713_tx_rds_buff(struct si4713_device *sdev, u8 mode, u16 rdsb,
				u16 rdsc, u16 rdsd, s8 *cbleft)
{
	int err;
	u8 val[SI4713_RDSBUFF_NRESP];

	const u8 args[SI4713_RDSBUFF_NARGS] = {
		mode & SI4713_RDSBUFF_MODE_MASK,
		msb(rdsb),
		lsb(rdsb),
		msb(rdsc),
		lsb(rdsc),
		msb(rdsd),
		lsb(rdsd),
	};

	err = si4713_send_command(sdev, SI4713_CMD_TX_RDS_BUFF,
				  args, ARRAY_SIZE(args), val,
				  ARRAY_SIZE(val), DEFAULT_TIMEOUT);

	if (!err) {
		v4l2_dbg(1, debug, &sdev->sd,
			"%s: status=0x%02x\n", __func__, val[0]);
		*cbleft = (s8)val[2] - val[3];
		v4l2_dbg(1, debug, &sdev->sd, "%s: response: interrupts"
				" 0x%02x cb avail: %d cb used %d fifo avail"
				" %d fifo used %d\n", __func__, val[1],
				val[2], val[3], val[4], val[5]);
	}

	return err;
}