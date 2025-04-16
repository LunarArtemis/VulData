static int si4713_setup(struct si4713_device *sdev)
{
	struct v4l2_ext_control ctrl;
	struct v4l2_frequency f;
	struct v4l2_modulator vm;
	struct si4713_device *tmp;
	int rval = 0;

	tmp = kmalloc(sizeof(*tmp), GFP_KERNEL);
	if (!tmp)
		return -ENOMEM;

	/* Get a local copy to avoid race */
	mutex_lock(&sdev->mutex);
	memcpy(tmp, sdev, sizeof(*sdev));
	mutex_unlock(&sdev->mutex);

	ctrl.id = V4L2_CID_RDS_TX_PI;
	ctrl.value = tmp->rds_info.pi;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_AUDIO_COMPRESSION_THRESHOLD;
	ctrl.value = tmp->acomp_info.threshold;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_AUDIO_COMPRESSION_GAIN;
	ctrl.value = tmp->acomp_info.gain;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_PILOT_TONE_FREQUENCY;
	ctrl.value = tmp->pilot_info.frequency;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_AUDIO_COMPRESSION_ATTACK_TIME;
	ctrl.value = tmp->acomp_info.attack_time;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_PILOT_TONE_DEVIATION;
	ctrl.value = tmp->pilot_info.deviation;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_AUDIO_LIMITER_DEVIATION;
	ctrl.value = tmp->limiter_info.deviation;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_RDS_TX_DEVIATION;
	ctrl.value = tmp->rds_info.deviation;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_RDS_TX_PTY;
	ctrl.value = tmp->rds_info.pty;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_AUDIO_LIMITER_ENABLED;
	ctrl.value = tmp->limiter_info.enabled;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_AUDIO_COMPRESSION_ENABLED;
	ctrl.value = tmp->acomp_info.enabled;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_PILOT_TONE_ENABLED;
	ctrl.value = tmp->pilot_info.enabled;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_AUDIO_LIMITER_RELEASE_TIME;
	ctrl.value = tmp->limiter_info.release_time;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_AUDIO_COMPRESSION_RELEASE_TIME;
	ctrl.value = tmp->acomp_info.release_time;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_TUNE_PREEMPHASIS;
	ctrl.value = tmp->preemphasis;
	rval |= si4713_write_econtrol_integers(sdev, &ctrl);

	ctrl.id = V4L2_CID_RDS_TX_PS_NAME;
	rval |= si4713_set_rds_ps_name(sdev, tmp->rds_info.ps_name);

	ctrl.id = V4L2_CID_RDS_TX_RADIO_TEXT;
	rval |= si4713_set_rds_radio_text(sdev, tmp->rds_info.radio_text);

	/* Device procedure needs to set frequency first */
	f.frequency = tmp->frequency ? tmp->frequency : DEFAULT_FREQUENCY;
	f.frequency = si4713_to_v4l2(f.frequency);
	rval |= si4713_s_frequency(&sdev->sd, &f);

	ctrl.id = V4L2_CID_TUNE_POWER_LEVEL;
	ctrl.value = tmp->power_level;
	rval |= si4713_write_econtrol_tune(sdev, &ctrl);

	ctrl.id = V4L2_CID_TUNE_ANTENNA_CAPACITOR;
	ctrl.value = tmp->antenna_capacitor;
	rval |= si4713_write_econtrol_tune(sdev, &ctrl);

	vm.index = 0;
	if (tmp->stereo)
		vm.txsubchans = V4L2_TUNER_SUB_STEREO;
	else
		vm.txsubchans = V4L2_TUNER_SUB_MONO;
	if (tmp->rds_info.enabled)
		vm.txsubchans |= V4L2_TUNER_SUB_RDS;
	si4713_s_modulator(&sdev->sd, &vm);

	kfree(tmp);

	return rval;
}