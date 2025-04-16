void
MavlinkReceiver::handle_message_debug_float_array(mavlink_message_t *msg)
{
	mavlink_debug_float_array_t debug_msg;
	mavlink_msg_debug_float_array_decode(msg, &debug_msg);

	debug_array_s debug_topic{};

	debug_topic.timestamp = hrt_absolute_time();
	debug_topic.id = debug_msg.array_id;
	memcpy(debug_topic.name, debug_msg.name, sizeof(debug_topic.name));
	debug_topic.name[sizeof(debug_topic.name) - 1] = '\0'; // enforce null termination

	for (size_t i = 0; i < debug_array_s::ARRAY_SIZE; i++) {
		debug_topic.data[i] = debug_msg.data[i];
	}

	_debug_array_pub.publish(debug_topic);
}