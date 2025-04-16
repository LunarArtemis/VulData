void
MavlinkReceiver::handle_message_command_int(mavlink_message_t *msg)
{
	/* command */
	mavlink_command_int_t cmd_mavlink;
	mavlink_msg_command_int_decode(msg, &cmd_mavlink);

	vehicle_command_s vcmd{};
	vcmd.timestamp = hrt_absolute_time();

	if (cmd_mavlink.x == 0x7ff80000 && cmd_mavlink.y == 0x7ff80000) {
		// This looks like NAN was by accident sent as int.
		PX4_ERR("x/y invalid of command %" PRIu16, cmd_mavlink.command);
		acknowledge(msg->sysid, msg->compid, cmd_mavlink.command, vehicle_command_ack_s::VEHICLE_RESULT_DENIED);
		return;
	}

	/* Copy the content of mavlink_command_int_t cmd_mavlink into command_t cmd */
	vcmd.param1 = cmd_mavlink.param1;
	vcmd.param2 = cmd_mavlink.param2;
	vcmd.param3 = cmd_mavlink.param3;
	vcmd.param4 = cmd_mavlink.param4;

	if (cmd_mavlink.x == INT32_MAX && cmd_mavlink.y == INT32_MAX) {
		// INT32_MAX for x and y means to ignore it.
		vcmd.param5 = (double)NAN;
		vcmd.param6 = (double)NAN;

	} else {
		vcmd.param5 = ((double)cmd_mavlink.x) / 1e7;
		vcmd.param6 = ((double)cmd_mavlink.y) / 1e7;
	}

	vcmd.param7 = cmd_mavlink.z;
	vcmd.command = cmd_mavlink.command;
	vcmd.target_system = cmd_mavlink.target_system;
	vcmd.target_component = cmd_mavlink.target_component;
	vcmd.source_system = msg->sysid;
	vcmd.source_component = msg->compid;
	vcmd.confirmation = false;
	vcmd.from_external = true;

	handle_message_command_both(msg, cmd_mavlink, vcmd);
}