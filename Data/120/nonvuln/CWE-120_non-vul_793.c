int
MavlinkReceiver::set_message_interval(int msgId, float interval, int data_rate)
{
	if (msgId == MAVLINK_MSG_ID_HEARTBEAT) {
		return PX4_ERROR;
	}

	if (data_rate > 0) {
		_mavlink->set_data_rate(data_rate);
	}

	// configure_stream wants a rate (msgs/second), so convert here.
	float rate = 0.f;

	if (interval < -0.00001f) {
		rate = 0.f; // stop the stream

	} else if (interval > 0.00001f) {
		rate = 1000000.0f / interval;

	} else {
		rate = -2.f; // set default rate
	}

	bool found_id = false;

	if (msgId != 0) {
		const char *stream_name = get_stream_name(msgId);

		if (stream_name != nullptr) {
			_mavlink->configure_stream_threadsafe(stream_name, rate);
			found_id = true;
		}
	}

	return (found_id ? PX4_OK : PX4_ERROR);
}