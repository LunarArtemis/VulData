void
MavlinkReceiver::handle_message_command_long(mavlink_message_t *msg)
{
	/* command */
	mavlink_command_long_t cmd_mavlink;
	mavlink_msg_command_long_decode(msg, &cmd_mavlink);

	vehicle_command_s vcmd{};

	vcmd.timestamp = hrt_absolute_time();

	const float before_int32_max = nextafterf((float)INT32_MAX, 0.0f);
	const float after_int32_max = nextafterf((float)INT32_MAX, (float)INFINITY);

	if (cmd_mavlink.param5 >= before_int32_max && cmd_mavlink.param5 <= after_int32_max &&
	    cmd_mavlink.param6 >= before_int32_max && cmd_mavlink.param6 <= after_int32_max) {
		// This looks suspicously like INT32_MAX was sent in a COMMAND_LONG instead of
		// a COMMAND_INT.
		PX4_ERR("param5/param6 invalid of command %" PRIu16, cmd_mavlink.command);
		acknowledge(msg->sysid, msg->compid, cmd_mavlink.command, vehicle_command_ack_s::VEHICLE_RESULT_DENIED);
		return;
	}

	/* Copy the content of mavlink_command_long_t cmd_mavlink into command_t cmd */
	vcmd.param1 = cmd_mavlink.param1;
	vcmd.param2 = cmd_mavlink.param2;
	vcmd.param3 = cmd_mavlink.param3;
	vcmd.param4 = cmd_mavlink.param4;
	vcmd.param5 = (double)cmd_mavlink.param5;
	vcmd.param6 = (double)cmd_mavlink.param6;
	vcmd.param7 = cmd_mavlink.param7;
	vcmd.command = cmd_mavlink.command;
	vcmd.target_system = cmd_mavlink.target_system;
	vcmd.target_component = cmd_mavlink.target_component;
	vcmd.source_system = msg->sysid;
	vcmd.source_component = msg->compid;
	vcmd.confirmation = cmd_mavlink.confirmation;
	vcmd.from_external = true;

	handle_message_command_both(msg, cmd_mavlink, vcmd);
}