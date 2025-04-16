void MavlinkReceiver::schedule_tune(const char *tune)
{
	// We only allocate the TunePublisher object if we ever use it but we
	// don't remove it to avoid fragmentation over time.
	if (_tune_publisher == nullptr) {
		_tune_publisher = new TunePublisher();

		if (_tune_publisher == nullptr) {
			PX4_ERR("Could not allocate tune publisher");
			return;
		}
	}

	const hrt_abstime now = hrt_absolute_time();

	_tune_publisher->set_tune_string(tune, now);
	// Send first one straightaway.
	_tune_publisher->publish_next_tune(now);
}