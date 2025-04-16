void
MavlinkReceiver::handle_message_rc_channels(mavlink_message_t *msg)
{
	mavlink_rc_channels_t rc_channels;
	mavlink_msg_rc_channels_decode(msg, &rc_channels);

	if (msg->compid != MAV_COMP_ID_SYSTEM_CONTROL) {
		PX4_DEBUG("Mavlink receiver only processes RC_CHANNELS from MAV_COMP_ID_SYSTEM_CONTROL");
		return;
	}

	input_rc_s rc{};

	rc.timestamp_last_signal = hrt_absolute_time();
	rc.rssi = input_rc_s::RSSI_MAX;

	// TODO: fake RSSI from dropped messages?
	// for (auto &component_state : _component_states) {
	// 	if (component_state.component_id == MAV_COMP_ID_SYSTEM_CONTROL) {
	// 		rc.rssi = (float)component_state.missed_messages / (float)component_state.received_messages;
	// 	}
	// }

	rc.rc_total_frame_count = 1;
	rc.input_source = input_rc_s::RC_INPUT_SOURCE_MAVLINK;

	// channels
	rc.values[0] = rc_channels.chan1_raw;
	rc.values[1] = rc_channels.chan2_raw;
	rc.values[2] = rc_channels.chan3_raw;
	rc.values[3] = rc_channels.chan4_raw;
	rc.values[4] = rc_channels.chan5_raw;
	rc.values[5] = rc_channels.chan6_raw;
	rc.values[6] = rc_channels.chan7_raw;
	rc.values[7] = rc_channels.chan8_raw;
	rc.values[8] = rc_channels.chan9_raw;
	rc.values[9] = rc_channels.chan10_raw;
	rc.values[10] = rc_channels.chan11_raw;
	rc.values[11] = rc_channels.chan12_raw;
	rc.values[12] = rc_channels.chan13_raw;
	rc.values[13] = rc_channels.chan14_raw;
	rc.values[14] = rc_channels.chan15_raw;
	rc.values[15] = rc_channels.chan16_raw;
	rc.values[16] = rc_channels.chan17_raw;
	rc.values[17] = rc_channels.chan18_raw;

	// check how many channels are valid
	for (int i = 17; i >= 0; i--) {
		const bool ignore_max = rc.values[i] == UINT16_MAX; // ignore any channel with value UINT16_MAX
		const bool ignore_zero = (i > 7) && (rc.values[i] == 0); // ignore channel 8-18 if value is 0

		if (ignore_max || ignore_zero) {
			// set all ignored values to zero
			rc.values[i] = 0;

		} else {
			// first channel to not ignore -> set count considering zero-based index
			rc.channel_count = i + 1;
			break;
		}
	}

	// publish uORB message
	rc.timestamp = hrt_absolute_time();
	_rc_pub.publish(rc);
}