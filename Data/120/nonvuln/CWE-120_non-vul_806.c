void
MavlinkReceiver::handle_message_debug(mavlink_message_t *msg)
{
	mavlink_debug_t debug_msg;
	mavlink_msg_debug_decode(msg, &debug_msg);

	debug_value_s debug_topic{};

	debug_topic.timestamp = hrt_absolute_time();
	debug_topic.ind = debug_msg.ind;
	debug_topic.value = debug_msg.value;

	_debug_value_pub.publish(debug_topic);
}