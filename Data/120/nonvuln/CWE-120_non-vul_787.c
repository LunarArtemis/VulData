uint8_t MavlinkReceiver::handle_request_message_command(uint16_t message_id, float param2, float param3, float param4,
		float param5, float param6, float param7)
{
	bool stream_found = false;
	bool message_sent = false;

	for (const auto &stream : _mavlink->get_streams()) {
		if (stream->get_id() == message_id) {
			stream_found = true;
			message_sent = stream->request_message(param2, param3, param4, param5, param6, param7);
			break;
		}
	}

	if (!stream_found) {
		// If we don't find the stream, we can configure it with rate 0 and then trigger it once.
		const char *stream_name = get_stream_name(message_id);

		if (stream_name != nullptr) {
			_mavlink->configure_stream_threadsafe(stream_name, 0.0f);

			// Now we try again to send it.
			for (const auto &stream : _mavlink->get_streams()) {
				if (stream->get_id() == message_id) {
					message_sent = stream->request_message(param2, param3, param4, param5, param6, param7);
					break;
				}
			}
		}
	}

	return (message_sent ? vehicle_command_ack_s::VEHICLE_RESULT_ACCEPTED : vehicle_command_ack_s::VEHICLE_RESULT_DENIED);
}