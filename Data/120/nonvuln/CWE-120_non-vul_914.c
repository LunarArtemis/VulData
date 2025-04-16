static int si4713_set_rds_radio_text(struct si4713_device *sdev, char *rt)
{
	int rval = 0, i;
	u16 t_index = 0;
	u8 b_index = 0, cr_inserted = 0;
	s8 left;

	mutex_lock(&sdev->mutex);

	if (!sdev->power_state)
		goto copy;

	rval = si4713_tx_rds_buff(sdev, RDS_BLOCK_CLEAR, 0, 0, 0, &left);
	if (rval < 0)
		goto unlock;

	if (!strlen(rt))
		goto copy;

	do {
		/* RDS spec says that if the last block isn't used,
		 * then apply a carriage return
		 */
		if (t_index < (RDS_RADIOTEXT_INDEX_MAX *
			RDS_RADIOTEXT_BLK_SIZE)) {
			for (i = 0; i < RDS_RADIOTEXT_BLK_SIZE; i++) {
				if (!rt[t_index + i] || rt[t_index + i] ==
					RDS_CARRIAGE_RETURN) {
					rt[t_index + i] = RDS_CARRIAGE_RETURN;
					cr_inserted = 1;
					break;
				}
			}
		}

		rval = si4713_tx_rds_buff(sdev, RDS_BLOCK_LOAD,
				compose_u16(RDS_RADIOTEXT_2A, b_index++),
				compose_u16(rt[t_index], rt[t_index + 1]),
				compose_u16(rt[t_index + 2], rt[t_index + 3]),
				&left);
		if (rval < 0)
			goto unlock;

		t_index += RDS_RADIOTEXT_BLK_SIZE;

		if (cr_inserted)
			break;
	} while (left > 0);

copy:
	strncpy(sdev->rds_info.radio_text, rt, MAX_RDS_RADIO_TEXT);

unlock:
	mutex_unlock(&sdev->mutex);
	return rval;
}