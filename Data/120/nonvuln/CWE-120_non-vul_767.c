void
MavlinkReceiver::handle_message_named_value_float(mavlink_message_t *msg)
{
	mavlink_named_value_float_t debug_msg;
	mavlink_msg_named_value_float_decode(msg, &debug_msg);

	debug_key_value_s debug_topic{};

	debug_topic.timestamp = hrt_absolute_time();
	memcpy(debug_topic.key, debug_msg.name, sizeof(debug_topic.key));
	debug_topic.key[sizeof(debug_topic.key) - 1] = '\0'; // enforce null termination
	debug_topic.value = debug_msg.value;

	_debug_key_value_pub.publish(debug_topic);
}