static int dfu_class_handle_req(struct usb_setup_packet *pSetup,
		s32_t *data_len, u8_t **data)
{
	int ret;
	u32_t len, bytes_left;

	switch (pSetup->bRequest) {
	case DFU_GETSTATUS:
		LOG_DBG("DFU_GETSTATUS: status %d, state %d",
			dfu_data.status, dfu_data.state);

		if (dfu_data.state == dfuMANIFEST_SYNC) {
			dfu_data.state = dfuIDLE;
		}

		/* bStatus */
		(*data)[0] = dfu_data.status;
		/* bwPollTimeout */
		sys_put_le16(dfu_data.bwPollTimeout, &(*data)[1]);
		(*data)[3] = 0U;
		/* bState */
		(*data)[4] = dfu_data.state;
		/* iString */
		(*data)[5] = 0U;
		*data_len = 6;
		break;

	case DFU_GETSTATE:
		LOG_DBG("DFU_GETSTATE");
		(*data)[0] = dfu_data.state;
		*data_len = 1;
		break;

	case DFU_ABORT:
		LOG_DBG("DFU_ABORT");

		if (dfu_check_app_state()) {
			return -EINVAL;
		}

		dfu_reset_counters();
		dfu_data.state = dfuIDLE;
		dfu_data.status = statusOK;
		break;

	case DFU_CLRSTATUS:
		LOG_DBG("DFU_CLRSTATUS");

		if (dfu_check_app_state()) {
			return -EINVAL;
		}

		dfu_data.state = dfuIDLE;
		dfu_data.status = statusOK;
		break;

	case DFU_DNLOAD:
		LOG_DBG("DFU_DNLOAD block %d, len %d, state %d",
			pSetup->wValue, pSetup->wLength, dfu_data.state);

		if (dfu_check_app_state()) {
			return -EINVAL;
		}

		switch (dfu_data.state) {
		case dfuIDLE:
			LOG_DBG("DFU_DNLOAD start");
			dfu_reset_counters();
			k_poll_signal_reset(&dfu_signal);

			if (dfu_data.flash_area_id !=
			    DT_FLASH_AREA_IMAGE_1_ID) {
				dfu_data.status = errWRITE;
				dfu_data.state = dfuERROR;
				LOG_ERR("This area can not be overwritten");
				break;
			}

			dfu_data.state = dfuDNBUSY;
			dfu_data_worker.worker_state = dfuIDLE;
			dfu_data_worker.worker_len  = pSetup->wLength;
			memcpy(dfu_data_worker.buf, *data, pSetup->wLength);
			k_work_submit(&dfu_work);
			break;
		case dfuDNLOAD_IDLE:
			dfu_data.state = dfuDNBUSY;
			dfu_data_worker.worker_state = dfuDNLOAD_IDLE;
			dfu_data_worker.worker_len  = pSetup->wLength;
			if (dfu_data_worker.worker_len == 0U) {
				dfu_data.state = dfuMANIFEST_SYNC;
				k_poll_signal_raise(&dfu_signal, 0);
			}

			memcpy(dfu_data_worker.buf, *data, pSetup->wLength);
			k_work_submit(&dfu_work);
			break;
		default:
			LOG_ERR("DFU_DNLOAD wrong state %d", dfu_data.state);
			dfu_data.state = dfuERROR;
			dfu_data.status = errUNKNOWN;
			dfu_reset_counters();
			return -EINVAL;
		}
		break;
	case DFU_UPLOAD:
		LOG_DBG("DFU_UPLOAD block %d, len %d, state %d",
			pSetup->wValue, pSetup->wLength, dfu_data.state);

		if (dfu_check_app_state()) {
			return -EINVAL;
		}

		switch (dfu_data.state) {
		case dfuIDLE:
			dfu_reset_counters();
			LOG_DBG("DFU_UPLOAD start");
		case dfuUPLOAD_IDLE:
			if (!pSetup->wLength ||
			    dfu_data.block_nr != pSetup->wValue) {
				LOG_DBG("DFU_UPLOAD block %d, expected %d, "
					"len %d", pSetup->wValue,
					dfu_data.block_nr, pSetup->wLength);
				dfu_data.state = dfuERROR;
				dfu_data.status = errUNKNOWN;
				break;
			}

			/* Upload in progress */
			bytes_left = dfu_data.flash_upload_size -
				     dfu_data.bytes_sent;
			if (bytes_left < pSetup->wLength) {
				len = bytes_left;
			} else {
				len = pSetup->wLength;
			}

			if (len) {
				const struct flash_area *fa;

				ret = flash_area_open(dfu_data.flash_area_id,
						      &fa);
				if (ret) {
					dfu_data.state = dfuERROR;
					dfu_data.status = errFILE;
					break;
				}
				ret = flash_area_read(fa, dfu_data.bytes_sent,
						      *data, len);
				flash_area_close(fa);
				if (ret) {
					dfu_data.state = dfuERROR;
					dfu_data.status = errFILE;
					break;
				}
			}
			*data_len = len;

			dfu_data.bytes_sent += len;
			dfu_data.block_nr++;

			if (dfu_data.bytes_sent == dfu_data.flash_upload_size &&
			    len < pSetup->wLength) {
				/* Upload completed when a
				 * short packet is received
				 */
				*data_len = 0;
				dfu_data.state = dfuIDLE;
			} else
				dfu_data.state = dfuUPLOAD_IDLE;

			break;
		default:
			LOG_ERR("DFU_UPLOAD wrong state %d", dfu_data.state);
			dfu_data.state = dfuERROR;
			dfu_data.status = errUNKNOWN;
			dfu_reset_counters();
			return -EINVAL;
		}
		break;
	case DFU_DETACH:
		LOG_DBG("DFU_DETACH timeout %d, state %d",
			pSetup->wValue, dfu_data.state);

		if (dfu_data.state != appIDLE) {
			dfu_data.state = appIDLE;
			return -EINVAL;
		}
		/* Move to appDETACH state */
		dfu_data.state = appDETACH;

		/* We should start a timer here but in order to
		 * keep things simple and do not increase the size
		 * we rely on the host to get us out of the appATTACHED
		 * state if needed.
		 */

		/* Set the DFU mode descriptors to be used after reset */
		dfu_config.usb_device_description = (u8_t *) &dfu_mode_desc;
		if (usb_set_config(dfu_config.usb_device_description) != 0) {
			LOG_ERR("usb_set_config failed in DFU_DETACH");
			return -EIO;
		}
		break;
	default:
		LOG_WRN("DFU UNKNOWN STATE: %d", pSetup->bRequest);
		return -EINVAL;
	}

	return 0;
}