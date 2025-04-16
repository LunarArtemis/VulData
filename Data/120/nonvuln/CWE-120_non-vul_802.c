void *MavlinkReceiver::start_trampoline(void *context)
{
	MavlinkReceiver *self = reinterpret_cast<MavlinkReceiver *>(context);
	self->run();
	return nullptr;
}