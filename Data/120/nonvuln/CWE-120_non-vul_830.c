void
MavlinkReceiver::handle_message_collision(mavlink_message_t *msg)
{
	mavlink_collision_t collision;
	mavlink_msg_collision_decode(msg, &collision);

	collision_report_s collision_report{};

	collision_report.timestamp = hrt_absolute_time();
	collision_report.src = collision.src;
	collision_report.id = collision.id;
	collision_report.action = collision.action;
	collision_report.threat_level = collision.threat_level;
	collision_report.time_to_minimum_delta = collision.time_to_minimum_delta;
	collision_report.altitude_minimum_delta = collision.altitude_minimum_delta;
	collision_report.horizontal_minimum_delta = collision.horizontal_minimum_delta;

	_collision_report_pub.publish(collision_report);
}