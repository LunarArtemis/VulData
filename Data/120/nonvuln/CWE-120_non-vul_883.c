static int si4713_choose_econtrol_action(struct si4713_device *sdev, u32 id,
		u32 **shadow, s32 *bit, s32 *mask, u16 *property, int *mul,
		unsigned long **table, int *size)
{
	s32 rval = 0;

	switch (id) {
	/* FM_TX class controls */
	case V4L2_CID_RDS_TX_PI:
		*property = SI4713_TX_RDS_PI;
		*mul = 1;
		*shadow = &sdev->rds_info.pi;
		break;
	case V4L2_CID_AUDIO_COMPRESSION_THRESHOLD:
		*property = SI4713_TX_ACOMP_THRESHOLD;
		*mul = 1;
		*shadow = &sdev->acomp_info.threshold;
		break;
	case V4L2_CID_AUDIO_COMPRESSION_GAIN:
		*property = SI4713_TX_ACOMP_GAIN;
		*mul = 1;
		*shadow = &sdev->acomp_info.gain;
		break;
	case V4L2_CID_PILOT_TONE_FREQUENCY:
		*property = SI4713_TX_PILOT_FREQUENCY;
		*mul = 1;
		*shadow = &sdev->pilot_info.frequency;
		break;
	case V4L2_CID_AUDIO_COMPRESSION_ATTACK_TIME:
		*property = SI4713_TX_ACOMP_ATTACK_TIME;
		*mul = ATTACK_TIME_UNIT;
		*shadow = &sdev->acomp_info.attack_time;
		break;
	case V4L2_CID_PILOT_TONE_DEVIATION:
		*property = SI4713_TX_PILOT_DEVIATION;
		*mul = 10;
		*shadow = &sdev->pilot_info.deviation;
		break;
	case V4L2_CID_AUDIO_LIMITER_DEVIATION:
		*property = SI4713_TX_AUDIO_DEVIATION;
		*mul = 10;
		*shadow = &sdev->limiter_info.deviation;
		break;
	case V4L2_CID_RDS_TX_DEVIATION:
		*property = SI4713_TX_RDS_DEVIATION;
		*mul = 1;
		*shadow = &sdev->rds_info.deviation;
		break;

	case V4L2_CID_RDS_TX_PTY:
		*property = SI4713_TX_RDS_PS_MISC;
		*bit = 5;
		*mask = 0x1F << 5;
		*shadow = &sdev->rds_info.pty;
		break;
	case V4L2_CID_AUDIO_LIMITER_ENABLED:
		*property = SI4713_TX_ACOMP_ENABLE;
		*bit = 1;
		*mask = 1 << 1;
		*shadow = &sdev->limiter_info.enabled;
		break;
	case V4L2_CID_AUDIO_COMPRESSION_ENABLED:
		*property = SI4713_TX_ACOMP_ENABLE;
		*bit = 0;
		*mask = 1 << 0;
		*shadow = &sdev->acomp_info.enabled;
		break;
	case V4L2_CID_PILOT_TONE_ENABLED:
		*property = SI4713_TX_COMPONENT_ENABLE;
		*bit = 0;
		*mask = 1 << 0;
		*shadow = &sdev->pilot_info.enabled;
		break;

	case V4L2_CID_AUDIO_LIMITER_RELEASE_TIME:
		*property = SI4713_TX_LIMITER_RELEASE_TIME;
		*table = limiter_times;
		*size = ARRAY_SIZE(limiter_times);
		*shadow = &sdev->limiter_info.release_time;
		break;
	case V4L2_CID_AUDIO_COMPRESSION_RELEASE_TIME:
		*property = SI4713_TX_ACOMP_RELEASE_TIME;
		*table = acomp_rtimes;
		*size = ARRAY_SIZE(acomp_rtimes);
		*shadow = &sdev->acomp_info.release_time;
		break;
	case V4L2_CID_TUNE_PREEMPHASIS:
		*property = SI4713_TX_PREEMPHASIS;
		*table = preemphasis_values;
		*size = ARRAY_SIZE(preemphasis_values);
		*shadow = &sdev->preemphasis;
		break;

	default:
		rval = -EINVAL;
	};

	return rval;
}