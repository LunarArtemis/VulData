void
MavlinkReceiver::handle_message_manual_control(mavlink_message_t *msg)
{
	mavlink_manual_control_t man;
	mavlink_msg_manual_control_decode(msg, &man);

	// Check target
	if (man.target != 0 && man.target != _mavlink->get_system_id()) {
		return;
	}

	if (_mavlink->should_generate_virtual_rc_input()) {

		input_rc_s rc{};
		rc.timestamp = hrt_absolute_time();
		rc.timestamp_last_signal = rc.timestamp;

		rc.channel_count = 8;
		rc.rc_failsafe = false;
		rc.rc_lost = false;
		rc.rc_lost_frame_count = 0;
		rc.rc_total_frame_count = 1;
		rc.rc_ppm_frame_length = 0;
		rc.input_source = input_rc_s::RC_INPUT_SOURCE_MAVLINK;
		rc.rssi = input_rc_s::RSSI_MAX;

		rc.values[0] = man.x / 2 + 1500;	// roll
		rc.values[1] = man.y / 2 + 1500;	// pitch
		rc.values[2] = man.r / 2 + 1500;	// yaw
		rc.values[3] = math::constrain(man.z / 0.9f + 800.0f, 1000.0f, 2000.0f);	// throttle

		/* decode all switches which fit into the channel mask */
		unsigned max_switch = (sizeof(man.buttons) * 8);
		unsigned max_channels = (sizeof(rc.values) / sizeof(rc.values[0]));

		if (max_switch > (max_channels - 4)) {
			max_switch = (max_channels - 4);
		}

		/* fill all channels */
		for (unsigned i = 0; i < max_switch; i++) {
			rc.values[i + 4] = decode_switch_pos_n(man.buttons, i);
		}

		_mom_switch_state = man.buttons;

		_rc_pub.publish(rc);

	} else {
		manual_control_setpoint_s manual{};

		manual.timestamp = hrt_absolute_time();
		manual.x = man.x / 1000.0f;
		manual.y = man.y / 1000.0f;
		manual.r = man.r / 1000.0f;
		manual.z = man.z / 1000.0f;
		manual.data_source = manual_control_setpoint_s::SOURCE_MAVLINK_0 + _mavlink->get_instance_id();

		_manual_control_setpoint_pub.publish(manual);
	}
}