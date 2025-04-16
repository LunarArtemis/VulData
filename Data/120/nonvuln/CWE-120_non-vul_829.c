void
MavlinkReceiver::handle_message_debug_vect(mavlink_message_t *msg)
{
	mavlink_debug_vect_t debug_msg;
	mavlink_msg_debug_vect_decode(msg, &debug_msg);

	debug_vect_s debug_topic{};

	debug_topic.timestamp = hrt_absolute_time();
	memcpy(debug_topic.name, debug_msg.name, sizeof(debug_topic.name));
	debug_topic.name[sizeof(debug_topic.name) - 1] = '\0'; // enforce null termination
	debug_topic.x = debug_msg.x;
	debug_topic.y = debug_msg.y;
	debug_topic.z = debug_msg.z;

	_debug_vect_pub.publish(debug_topic);
}