void
MavlinkReceiver::handle_message_logging_ack(mavlink_message_t *msg)
{
	mavlink_logging_ack_t logging_ack;
	mavlink_msg_logging_ack_decode(msg, &logging_ack);

	MavlinkULog *ulog_streaming = _mavlink->get_ulog_streaming();

	if (ulog_streaming) {
		ulog_streaming->handle_ack(logging_ack);
	}
}