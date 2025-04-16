static int si4713_set_power_state(struct si4713_device *sdev, u8 value)
{
	int rval;

	mutex_lock(&sdev->mutex);

	if (value)
		rval = si4713_powerup(sdev);
	else
		rval = si4713_powerdown(sdev);

	mutex_unlock(&sdev->mutex);
	return rval;
}