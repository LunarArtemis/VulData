void
MavlinkReceiver::handle_message_play_tune(mavlink_message_t *msg)
{
	mavlink_play_tune_t play_tune;
	mavlink_msg_play_tune_decode(msg, &play_tune);

	if ((mavlink_system.sysid == play_tune.target_system || play_tune.target_system == 0) &&
	    (mavlink_system.compid == play_tune.target_component || play_tune.target_component == 0)) {

		// Let's make sure the input is 0 terminated, so we don't ever overrun it.
		play_tune.tune2[sizeof(play_tune.tune2) - 1] = '\0';

		schedule_tune(play_tune.tune);
	}
}