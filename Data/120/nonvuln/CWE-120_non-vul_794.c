void
MavlinkReceiver::handle_message_follow_target(mavlink_message_t *msg)
{
	mavlink_follow_target_t follow_target_msg;
	mavlink_msg_follow_target_decode(msg, &follow_target_msg);

	follow_target_s follow_target_topic{};

	follow_target_topic.timestamp = hrt_absolute_time();
	follow_target_topic.lat = follow_target_msg.lat * 1e-7;
	follow_target_topic.lon = follow_target_msg.lon * 1e-7;
	follow_target_topic.alt = follow_target_msg.alt;
	follow_target_topic.vx = follow_target_msg.vel[0];
	follow_target_topic.vy = follow_target_msg.vel[1];
	follow_target_topic.vz = follow_target_msg.vel[2];

	_follow_target_pub.publish(follow_target_topic);
}