void
MavlinkReceiver::handle_message_set_actuator_control_target(mavlink_message_t *msg)
{
	// TODO
#if defined(ENABLE_LOCKSTEP_SCHEDULER)
	PX4_ERR("SET_ACTUATOR_CONTROL_TARGET not supported with lockstep enabled");
	PX4_ERR("Please disable lockstep for actuator offboard control:");
	PX4_ERR("https://dev.px4.io/master/en/simulation/#disable-lockstep-simulation");
	return;
#endif

	mavlink_set_actuator_control_target_t actuator_target;
	mavlink_msg_set_actuator_control_target_decode(msg, &actuator_target);

	if (_mavlink->get_forward_externalsp() &&
	    (mavlink_system.sysid == actuator_target.target_system || actuator_target.target_system == 0) &&
	    (mavlink_system.compid == actuator_target.target_component || actuator_target.target_component == 0)
	   ) {
		/* Ignore all setpoints except when controlling the gimbal(group_mlx==2) as we are setting raw actuators here */
		//bool ignore_setpoints = bool(actuator_target.group_mlx != 2);

		offboard_control_mode_s offboard_control_mode{};
		offboard_control_mode.timestamp = hrt_absolute_time();
		_offboard_control_mode_pub.publish(offboard_control_mode);

		vehicle_status_s vehicle_status{};
		_vehicle_status_sub.copy(&vehicle_status);

		// Publish actuator controls only once in OFFBOARD
		if (vehicle_status.nav_state == vehicle_status_s::NAVIGATION_STATE_OFFBOARD) {

			actuator_controls_s actuator_controls{};
			actuator_controls.timestamp = hrt_absolute_time();

			/* Set duty cycles for the servos in the actuator_controls message */
			for (size_t i = 0; i < 8; i++) {
				actuator_controls.control[i] = actuator_target.controls[i];
			}

			switch (actuator_target.group_mlx) {
			case 0:
				_actuator_controls_pubs[0].publish(actuator_controls);
				break;

			case 1:
				_actuator_controls_pubs[1].publish(actuator_controls);
				break;

			case 2:
				_actuator_controls_pubs[2].publish(actuator_controls);
				break;

			case 3:
				_actuator_controls_pubs[3].publish(actuator_controls);
				break;

			default:
				break;
			}
		}
	}
}