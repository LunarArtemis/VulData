static int si4713_set_rds_ps_name(struct si4713_device *sdev, char *ps_name)
{
	int rval = 0, i;
	u8 len = 0;

	/* We want to clear the whole thing */
	if (!strlen(ps_name))
		memset(ps_name, 0, MAX_RDS_PS_NAME + 1);

	mutex_lock(&sdev->mutex);

	if (sdev->power_state) {
		/* Write the new ps name and clear the padding */
		for (i = 0; i < MAX_RDS_PS_NAME; i += (RDS_BLOCK / 2)) {
			rval = si4713_tx_rds_ps(sdev, (i / (RDS_BLOCK / 2)),
						ps_name + i);
			if (rval < 0)
				goto unlock;
		}

		/* Setup the size to be sent */
		if (strlen(ps_name))
			len = strlen(ps_name) - 1;
		else
			len = 1;

		rval = si4713_write_property(sdev,
				SI4713_TX_RDS_PS_MESSAGE_COUNT,
				rds_ps_nblocks(len));
		if (rval < 0)
			goto unlock;

		rval = si4713_write_property(sdev,
				SI4713_TX_RDS_PS_REPEAT_COUNT,
				DEFAULT_RDS_PS_REPEAT_COUNT * 2);
		if (rval < 0)
			goto unlock;
	}

	strncpy(sdev->rds_info.ps_name, ps_name, MAX_RDS_PS_NAME);

unlock:
	mutex_unlock(&sdev->mutex);
	return rval;
}