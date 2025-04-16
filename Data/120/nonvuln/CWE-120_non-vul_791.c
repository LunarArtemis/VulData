void
MavlinkReceiver::get_message_interval(int msgId)
{
	unsigned interval = 0;

	for (const auto &stream : _mavlink->get_streams()) {
		if (stream->get_id() == msgId) {
			interval = stream->get_interval();
			break;
		}
	}

	// send back this value...
	mavlink_msg_message_interval_send(_mavlink->get_channel(), msgId, interval);
}