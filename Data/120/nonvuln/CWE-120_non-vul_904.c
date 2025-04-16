static int si4713_queryctrl(struct v4l2_subdev *sd, struct v4l2_queryctrl *qc)
{
	int rval = 0;

	switch (qc->id) {
	/* User class controls */
	case V4L2_CID_AUDIO_MUTE:
		rval = v4l2_ctrl_query_fill(qc, 0, 1, 1, DEFAULT_MUTE);
		break;
	/* FM_TX class controls */
	case V4L2_CID_RDS_TX_PI:
		rval = v4l2_ctrl_query_fill(qc, 0, 0xFFFF, 1, DEFAULT_RDS_PI);
		break;
	case V4L2_CID_RDS_TX_PTY:
		rval = v4l2_ctrl_query_fill(qc, 0, 31, 1, DEFAULT_RDS_PTY);
		break;
	case V4L2_CID_RDS_TX_DEVIATION:
		rval = v4l2_ctrl_query_fill(qc, 0, MAX_RDS_DEVIATION,
						10, DEFAULT_RDS_DEVIATION);
		break;
	case V4L2_CID_RDS_TX_PS_NAME:
		/*
		 * Report step as 8. From RDS spec, psname
		 * should be 8. But there are receivers which scroll strings
		 * sized as 8xN.
		 */
		rval = v4l2_ctrl_query_fill(qc, 0, MAX_RDS_PS_NAME, 8, 0);
		break;
	case V4L2_CID_RDS_TX_RADIO_TEXT:
		/*
		 * Report step as 32 (2A block). From RDS spec,
		 * radio text should be 32 for 2A block. But there are receivers
		 * which scroll strings sized as 32xN. Setting default to 32.
		 */
		rval = v4l2_ctrl_query_fill(qc, 0, MAX_RDS_RADIO_TEXT, 32, 0);
		break;

	case V4L2_CID_AUDIO_LIMITER_ENABLED:
		rval = v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
		break;
	case V4L2_CID_AUDIO_LIMITER_RELEASE_TIME:
		rval = v4l2_ctrl_query_fill(qc, 250, MAX_LIMITER_RELEASE_TIME,
						50, DEFAULT_LIMITER_RTIME);
		break;
	case V4L2_CID_AUDIO_LIMITER_DEVIATION:
		rval = v4l2_ctrl_query_fill(qc, 0, MAX_LIMITER_DEVIATION,
						10, DEFAULT_LIMITER_DEV);
		break;

	case V4L2_CID_AUDIO_COMPRESSION_ENABLED:
		rval = v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
		break;
	case V4L2_CID_AUDIO_COMPRESSION_GAIN:
		rval = v4l2_ctrl_query_fill(qc, 0, MAX_ACOMP_GAIN, 1,
						DEFAULT_ACOMP_GAIN);
		break;
	case V4L2_CID_AUDIO_COMPRESSION_THRESHOLD:
		rval = v4l2_ctrl_query_fill(qc, MIN_ACOMP_THRESHOLD,
						MAX_ACOMP_THRESHOLD, 1,
						DEFAULT_ACOMP_THRESHOLD);
		break;
	case V4L2_CID_AUDIO_COMPRESSION_ATTACK_TIME:
		rval = v4l2_ctrl_query_fill(qc, 0, MAX_ACOMP_ATTACK_TIME,
						500, DEFAULT_ACOMP_ATIME);
		break;
	case V4L2_CID_AUDIO_COMPRESSION_RELEASE_TIME:
		rval = v4l2_ctrl_query_fill(qc, 100000, MAX_ACOMP_RELEASE_TIME,
						100000, DEFAULT_ACOMP_RTIME);
		break;

	case V4L2_CID_PILOT_TONE_ENABLED:
		rval = v4l2_ctrl_query_fill(qc, 0, 1, 1, 1);
		break;
	case V4L2_CID_PILOT_TONE_DEVIATION:
		rval = v4l2_ctrl_query_fill(qc, 0, MAX_PILOT_DEVIATION,
						10, DEFAULT_PILOT_DEVIATION);
		break;
	case V4L2_CID_PILOT_TONE_FREQUENCY:
		rval = v4l2_ctrl_query_fill(qc, 0, MAX_PILOT_FREQUENCY,
						1, DEFAULT_PILOT_FREQUENCY);
		break;

	case V4L2_CID_TUNE_PREEMPHASIS:
		rval = v4l2_ctrl_query_fill(qc, V4L2_PREEMPHASIS_DISABLED,
						V4L2_PREEMPHASIS_75_uS, 1,
						V4L2_PREEMPHASIS_50_uS);
		break;
	case V4L2_CID_TUNE_POWER_LEVEL:
		rval = v4l2_ctrl_query_fill(qc, 0, 120, 1, DEFAULT_POWER_LEVEL);
		break;
	case V4L2_CID_TUNE_ANTENNA_CAPACITOR:
		rval = v4l2_ctrl_query_fill(qc, 0, 191, 1, 0);
		break;
	default:
		rval = -EINVAL;
		break;
	};

	return rval;
}