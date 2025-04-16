static int si4713_checkrev(struct si4713_device *sdev)
{
	struct i2c_client *client = v4l2_get_subdevdata(&sdev->sd);
	int rval;
	u8 resp[SI4713_GETREV_NRESP];

	mutex_lock(&sdev->mutex);

	rval = si4713_send_command(sdev, SI4713_CMD_GET_REV,
					NULL, 0,
					resp, ARRAY_SIZE(resp),
					DEFAULT_TIMEOUT);

	if (rval < 0)
		goto unlock;

	if (resp[1] == SI4713_PRODUCT_NUMBER) {
		v4l2_info(&sdev->sd, "chip found @ 0x%02x (%s)\n",
				client->addr << 1, client->adapter->name);
	} else {
		v4l2_err(&sdev->sd, "Invalid product number\n");
		rval = -EINVAL;
	}

unlock:
	mutex_unlock(&sdev->mutex);
	return rval;
}