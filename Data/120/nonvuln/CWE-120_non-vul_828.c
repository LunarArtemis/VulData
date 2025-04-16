void
MavlinkReceiver::handle_message_set_gps_global_origin(mavlink_message_t *msg)
{
	mavlink_set_gps_global_origin_t gps_global_origin;
	mavlink_msg_set_gps_global_origin_decode(msg, &gps_global_origin);

	if (gps_global_origin.target_system == _mavlink->get_system_id()) {
		vehicle_command_s vcmd{};
		vcmd.param5 = (double)gps_global_origin.latitude * 1.e-7;
		vcmd.param6 = (double)gps_global_origin.longitude * 1.e-7;
		vcmd.param7 = (float)gps_global_origin.altitude * 1.e-3f;
		vcmd.command = vehicle_command_s::VEHICLE_CMD_SET_GPS_GLOBAL_ORIGIN;
		vcmd.target_system = _mavlink->get_system_id();
		vcmd.target_component = MAV_COMP_ID_ALL;
		vcmd.source_system = msg->sysid;
		vcmd.source_component = msg->compid;
		vcmd.confirmation = false;
		vcmd.from_external = true;
		vcmd.timestamp = hrt_absolute_time();
		_cmd_pub.publish(vcmd);
	}

	handle_request_message_command(MAVLINK_MSG_ID_GPS_GLOBAL_ORIGIN);
}