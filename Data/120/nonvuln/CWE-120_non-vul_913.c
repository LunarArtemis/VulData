static int si4713_tx_rds_ps(struct si4713_device *sdev, u8 psid,
				unsigned char *pschar)
{
	int err;
	u8 val[SI4713_RDSPS_NRESP];

	const u8 args[SI4713_RDSPS_NARGS] = {
		psid & SI4713_RDSPS_PSID_MASK,
		pschar[0],
		pschar[1],
		pschar[2],
		pschar[3],
	};

	err = si4713_send_command(sdev, SI4713_CMD_TX_RDS_PS,
				  args, ARRAY_SIZE(args), val,
				  ARRAY_SIZE(val), DEFAULT_TIMEOUT);

	if (err < 0)
		return err;

	v4l2_dbg(1, debug, &sdev->sd, "%s: status=0x%02x\n", __func__, val[0]);

	return err;
}