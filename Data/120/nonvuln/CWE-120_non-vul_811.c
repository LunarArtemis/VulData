MavlinkReceiver::~MavlinkReceiver()
{
	delete _tune_publisher;
	delete _px4_accel;
	delete _px4_baro;
	delete _px4_gyro;
	delete _px4_mag;
#if !defined(CONSTRAINED_FLASH)
	delete[] _received_msg_stats;
#endif // !CONSTRAINED_FLASH
}