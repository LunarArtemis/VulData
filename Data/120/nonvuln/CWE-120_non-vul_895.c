static int si4713_read_econtrol_integers(struct si4713_device *sdev,
				struct v4l2_ext_control *control)
{
	s32 rval;
	u32 *shadow = NULL, val = 0;
	s32 bit = 0, mask = 0;
	u16 property = 0;
	int mul = 0;
	unsigned long *table = NULL;
	int size = 0;

	rval = si4713_choose_econtrol_action(sdev, control->id, &shadow, &bit,
			&mask, &property, &mul, &table, &size);
	if (rval < 0)
		goto exit;

	mutex_lock(&sdev->mutex);

	if (sdev->power_state) {
		rval = si4713_read_property(sdev, property, &val);
		if (rval < 0)
			goto unlock;

		/* Keep negative values for threshold */
		if (control->id == V4L2_CID_AUDIO_COMPRESSION_THRESHOLD)
			*shadow = (s16)val;
		else if (mask)
			*shadow = get_status_bit(val, bit, mask);
		else if (mul)
			*shadow = val * mul;
		else
			*shadow = dev_to_usecs(val, table, size);
	}

	control->value = *shadow;

unlock:
	mutex_unlock(&sdev->mutex);
exit:
	return rval;
}