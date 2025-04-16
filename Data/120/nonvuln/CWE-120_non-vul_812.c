void MavlinkReceiver::handle_message_request_event(mavlink_message_t *msg)
{
	_mavlink->get_events_protocol().handle_request_event(*msg);
}