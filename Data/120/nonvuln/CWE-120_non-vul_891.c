static int si4713_initialize(struct si4713_device *sdev)
{
	int rval;

	rval = si4713_set_power_state(sdev, POWER_ON);
	if (rval < 0)
		goto exit;

	rval = si4713_checkrev(sdev);
	if (rval < 0)
		goto exit;

	rval = si4713_set_power_state(sdev, POWER_OFF);
	if (rval < 0)
		goto exit;

	mutex_lock(&sdev->mutex);

	sdev->rds_info.pi = DEFAULT_RDS_PI;
	sdev->rds_info.pty = DEFAULT_RDS_PTY;
	sdev->rds_info.deviation = DEFAULT_RDS_DEVIATION;
	strlcpy(sdev->rds_info.ps_name, DEFAULT_RDS_PS_NAME, MAX_RDS_PS_NAME);
	strlcpy(sdev->rds_info.radio_text, DEFAULT_RDS_RADIO_TEXT,
							MAX_RDS_RADIO_TEXT);
	sdev->rds_info.enabled = 1;

	sdev->limiter_info.release_time = DEFAULT_LIMITER_RTIME;
	sdev->limiter_info.deviation = DEFAULT_LIMITER_DEV;
	sdev->limiter_info.enabled = 1;

	sdev->pilot_info.deviation = DEFAULT_PILOT_DEVIATION;
	sdev->pilot_info.frequency = DEFAULT_PILOT_FREQUENCY;
	sdev->pilot_info.enabled = 1;

	sdev->acomp_info.release_time = DEFAULT_ACOMP_RTIME;
	sdev->acomp_info.attack_time = DEFAULT_ACOMP_ATIME;
	sdev->acomp_info.threshold = DEFAULT_ACOMP_THRESHOLD;
	sdev->acomp_info.gain = DEFAULT_ACOMP_GAIN;
	sdev->acomp_info.enabled = 1;

	sdev->frequency = DEFAULT_FREQUENCY;
	sdev->preemphasis = DEFAULT_PREEMPHASIS;
	sdev->mute = DEFAULT_MUTE;
	sdev->power_level = DEFAULT_POWER_LEVEL;
	sdev->antenna_capacitor = 0;
	sdev->stereo = 1;
	sdev->tune_rnl = DEFAULT_TUNE_RNL;

	mutex_unlock(&sdev->mutex);

exit:
	return rval;
}