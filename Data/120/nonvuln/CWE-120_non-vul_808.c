void
MavlinkReceiver::handle_message_set_attitude_target(mavlink_message_t *msg)
{
	mavlink_set_attitude_target_t attitude_target;
	mavlink_msg_set_attitude_target_decode(msg, &attitude_target);

	/* Only accept messages which are intended for this system */
	if (_mavlink->get_forward_externalsp() &&
	    (mavlink_system.sysid == attitude_target.target_system || attitude_target.target_system == 0) &&
	    (mavlink_system.compid == attitude_target.target_component || attitude_target.target_component == 0)) {

		const uint8_t type_mask = attitude_target.type_mask;

		const bool attitude = !(type_mask & ATTITUDE_TARGET_TYPEMASK_ATTITUDE_IGNORE);
		const bool body_rates = !(type_mask & ATTITUDE_TARGET_TYPEMASK_BODY_ROLL_RATE_IGNORE)
					&& !(type_mask & ATTITUDE_TARGET_TYPEMASK_BODY_PITCH_RATE_IGNORE);
		const bool thrust_body = (type_mask & ATTITUDE_TARGET_TYPEMASK_THRUST_BODY_SET);

		vehicle_status_s vehicle_status{};
		_vehicle_status_sub.copy(&vehicle_status);

		if (attitude) {
			vehicle_attitude_setpoint_s attitude_setpoint{};

			const matrix::Quatf q{attitude_target.q};
			q.copyTo(attitude_setpoint.q_d);

			matrix::Eulerf euler{q};
			attitude_setpoint.roll_body = euler.phi();
			attitude_setpoint.pitch_body = euler.theta();
			attitude_setpoint.yaw_body = euler.psi();

			// TODO: review use case
			attitude_setpoint.yaw_sp_move_rate = (type_mask & ATTITUDE_TARGET_TYPEMASK_BODY_YAW_RATE_IGNORE) ?
							     (float)NAN : attitude_target.body_yaw_rate;

			if (!thrust_body && !(attitude_target.type_mask & ATTITUDE_TARGET_TYPEMASK_THROTTLE_IGNORE)) {
				fill_thrust(attitude_setpoint.thrust_body, vehicle_status.vehicle_type, attitude_target.thrust);

			} else if (thrust_body) {
				attitude_setpoint.thrust_body[0] = attitude_target.thrust_body[0];
				attitude_setpoint.thrust_body[1] = attitude_target.thrust_body[1];
				attitude_setpoint.thrust_body[2] = attitude_target.thrust_body[2];
			}

			// publish offboard_control_mode
			offboard_control_mode_s ocm{};
			ocm.attitude = true;
			ocm.timestamp = hrt_absolute_time();
			_offboard_control_mode_pub.publish(ocm);

			// Publish attitude setpoint only once in OFFBOARD
			if (vehicle_status.nav_state == vehicle_status_s::NAVIGATION_STATE_OFFBOARD) {
				attitude_setpoint.timestamp = hrt_absolute_time();

				if (vehicle_status.is_vtol && (vehicle_status.vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING)) {
					_mc_virtual_att_sp_pub.publish(attitude_setpoint);

				} else if (vehicle_status.is_vtol && (vehicle_status.vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING)) {
					_fw_virtual_att_sp_pub.publish(attitude_setpoint);

				} else {
					_att_sp_pub.publish(attitude_setpoint);
				}
			}

		} else if (body_rates) {
			vehicle_rates_setpoint_s setpoint{};
			setpoint.roll  = (type_mask & ATTITUDE_TARGET_TYPEMASK_BODY_ROLL_RATE_IGNORE)  ? (float)NAN :
					 attitude_target.body_roll_rate;
			setpoint.pitch = (type_mask & ATTITUDE_TARGET_TYPEMASK_BODY_PITCH_RATE_IGNORE) ? (float)NAN :
					 attitude_target.body_pitch_rate;
			setpoint.yaw   = (type_mask & ATTITUDE_TARGET_TYPEMASK_BODY_YAW_RATE_IGNORE)   ? (float)NAN :
					 attitude_target.body_yaw_rate;

			if (!(attitude_target.type_mask & ATTITUDE_TARGET_TYPEMASK_THROTTLE_IGNORE)) {
				fill_thrust(setpoint.thrust_body, vehicle_status.vehicle_type, attitude_target.thrust);
			}

			// publish offboard_control_mode
			offboard_control_mode_s ocm{};
			ocm.body_rate = true;
			ocm.timestamp = hrt_absolute_time();
			_offboard_control_mode_pub.publish(ocm);

			// Publish rate setpoint only once in OFFBOARD
			if (vehicle_status.nav_state == vehicle_status_s::NAVIGATION_STATE_OFFBOARD) {
				setpoint.timestamp = hrt_absolute_time();
				_rates_sp_pub.publish(setpoint);
			}
		}
	}
}