void MavlinkReceiver::stop()
{
	_should_exit.store(true);
	pthread_join(_thread, nullptr);
}