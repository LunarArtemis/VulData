static int si4713_g_frequency(struct v4l2_subdev *sd, struct v4l2_frequency *f)
{
	struct si4713_device *sdev = to_si4713_device(sd);
	int rval = 0;

	f->type = V4L2_TUNER_RADIO;

	mutex_lock(&sdev->mutex);

	if (sdev->power_state) {
		u16 freq;
		u8 p, a, n;

		rval = si4713_tx_tune_status(sdev, 0x00, &freq, &p, &a, &n);
		if (rval < 0)
			goto unlock;

		sdev->frequency = freq;
	}

	f->frequency = si4713_to_v4l2(sdev->frequency);

unlock:
	mutex_unlock(&sdev->mutex);
	return rval;
}