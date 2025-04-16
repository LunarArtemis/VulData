static int si4713_s_frequency(struct v4l2_subdev *sd, struct v4l2_frequency *f)
{
	struct si4713_device *sdev = to_si4713_device(sd);
	int rval = 0;
	u16 frequency = v4l2_to_si4713(f->frequency);

	/* Check frequency range */
	if (frequency < FREQ_RANGE_LOW || frequency > FREQ_RANGE_HIGH)
		return -EDOM;

	mutex_lock(&sdev->mutex);

	if (sdev->power_state) {
		rval = si4713_tx_tune_freq(sdev, frequency);
		if (rval < 0)
			goto unlock;
		frequency = rval;
		rval = 0;
	}
	sdev->frequency = frequency;
	f->frequency = si4713_to_v4l2(frequency);

unlock:
	mutex_unlock(&sdev->mutex);
	return rval;
}