static int si4713_write_econtrol_integers(struct si4713_device *sdev,
					struct v4l2_ext_control *control)
{
	s32 rval;
	u32 *shadow = NULL, val = 0;
	s32 bit = 0, mask = 0;
	u16 property = 0;
	int mul = 0;
	unsigned long *table = NULL;
	int size = 0;

	rval = validate_range(&sdev->sd, control);
	if (rval < 0)
		goto exit;

	rval = si4713_choose_econtrol_action(sdev, control->id, &shadow, &bit,
			&mask, &property, &mul, &table, &size);
	if (rval < 0)
		goto exit;

	val = control->value;
	if (mul) {
		val = control->value / mul;
	} else if (table) {
		rval = usecs_to_dev(control->value, table, size);
		if (rval < 0)
			goto exit;
		val = rval;
		rval = 0;
	}

	mutex_lock(&sdev->mutex);

	if (sdev->power_state) {
		if (mask) {
			rval = si4713_read_property(sdev, property, &val);
			if (rval < 0)
				goto unlock;
			val = set_bits(val, control->value, bit, mask);
		}

		rval = si4713_write_property(sdev, property, val);
		if (rval < 0)
			goto unlock;
		if (mask)
			val = control->value;
	}

	if (mul) {
		*shadow = val * mul;
	} else if (table) {
		rval = dev_to_usecs(val, table, size);
		if (rval < 0)
			goto unlock;
		*shadow = rval;
		rval = 0;
	} else {
		*shadow = val;
	}

unlock:
	mutex_unlock(&sdev->mutex);
exit:
	return rval;
}