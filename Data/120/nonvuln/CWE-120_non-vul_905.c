static int si4713_send_command(struct si4713_device *sdev, const u8 command,
				const u8 args[], const int argn,
				u8 response[], const int respn, const int usecs)
{
	struct i2c_client *client = v4l2_get_subdevdata(&sdev->sd);
	u8 data1[MAX_ARGS + 1];
	int err;

	if (!client->adapter)
		return -ENODEV;

	/* First send the command and its arguments */
	data1[0] = command;
	memcpy(data1 + 1, args, argn);
	DBG_BUFFER(&sdev->sd, "Parameters", data1, argn + 1);

	err = i2c_master_send(client, data1, argn + 1);
	if (err != argn + 1) {
		v4l2_err(&sdev->sd, "Error while sending command 0x%02x\n",
			command);
		return (err > 0) ? -EIO : err;
	}

	/* Wait response from interrupt */
	if (!wait_for_completion_timeout(&sdev->work,
				usecs_to_jiffies(usecs) + 1))
		v4l2_warn(&sdev->sd,
				"(%s) Device took too much time to answer.\n",
				__func__);

	/* Then get the response */
	err = i2c_master_recv(client, response, respn);
	if (err != respn) {
		v4l2_err(&sdev->sd,
			"Error while reading response for command 0x%02x\n",
			command);
		return (err > 0) ? -EIO : err;
	}

	DBG_BUFFER(&sdev->sd, "Response", response, respn);
	if (check_command_failed(response[0]))
		return -EBUSY;

	return 0;
}