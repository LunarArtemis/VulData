static int si4713_set_mute(struct si4713_device *sdev, u16 mute)
{
	int rval = 0;

	mute = set_mute(mute);

	mutex_lock(&sdev->mutex);

	if (sdev->power_state)
		rval = si4713_write_property(sdev,
				SI4713_TX_LINE_INPUT_MUTE, mute);

	if (rval >= 0)
		sdev->mute = get_mute(mute);

	mutex_unlock(&sdev->mutex);

	return rval;
}