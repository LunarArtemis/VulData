void
MavlinkReceiver::handle_message_play_tune_v2(mavlink_message_t *msg)
{
	mavlink_play_tune_v2_t play_tune_v2;
	mavlink_msg_play_tune_v2_decode(msg, &play_tune_v2);

	if ((mavlink_system.sysid == play_tune_v2.target_system || play_tune_v2.target_system == 0) &&
	    (mavlink_system.compid == play_tune_v2.target_component || play_tune_v2.target_component == 0)) {

		if (play_tune_v2.format != TUNE_FORMAT_QBASIC1_1) {
			PX4_ERR("Tune format %" PRIu32 " not supported", play_tune_v2.format);
			return;
		}

		// Let's make sure the input is 0 terminated, so we don't ever overrun it.
		play_tune_v2.tune[sizeof(play_tune_v2.tune) - 1] = '\0';

		schedule_tune(play_tune_v2.tune);
	}
}