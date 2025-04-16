static int si4713_wait_stc(struct si4713_device *sdev, const int usecs)
{
	int err;
	u8 resp[SI4713_GET_STATUS_NRESP];

	/* Wait response from STC interrupt */
	if (!wait_for_completion_timeout(&sdev->work,
			usecs_to_jiffies(usecs) + 1))
		v4l2_warn(&sdev->sd,
			"%s: device took too much time to answer (%d usec).\n",
				__func__, usecs);

	/* Clear status bits */
	err = si4713_send_command(sdev, SI4713_CMD_GET_INT_STATUS,
					NULL, 0,
					resp, ARRAY_SIZE(resp),
					DEFAULT_TIMEOUT);

	if (err < 0)
		goto exit;

	v4l2_dbg(1, debug, &sdev->sd,
			"%s: status bits: 0x%02x\n", __func__, resp[0]);

	if (!(resp[0] & SI4713_STC_INT))
		err = -EIO;

exit:
	return err;
}