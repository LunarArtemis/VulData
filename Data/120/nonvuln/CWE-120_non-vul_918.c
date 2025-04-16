static int si4713_update_tune_status(struct si4713_device *sdev)
{
	int rval;
	u16 f = 0;
	u8 p = 0, a = 0, n = 0;

	rval = si4713_tx_tune_status(sdev, 0x00, &f, &p, &a, &n);

	if (rval < 0)
		goto exit;

	sdev->power_level = p;
	sdev->antenna_capacitor = a;
	sdev->tune_rnl = n;

exit:
	return rval;
}