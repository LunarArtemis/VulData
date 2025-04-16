void MavlinkReceiver::update_message_statistics(const mavlink_message_t &message)
{
#if !defined(CONSTRAINED_FLASH)

	if (_received_msg_stats == nullptr) {
		_received_msg_stats = new ReceivedMessageStats[MAX_MSG_STAT_SLOTS];
	}

	if (_received_msg_stats) {
		const hrt_abstime now_ms = hrt_absolute_time() / 1000;

		int msg_stats_slot = -1;
		bool reset_stats = false;

		// find matching msg id
		for (int stat_slot = 0; stat_slot < MAX_MSG_STAT_SLOTS; stat_slot++) {
			if ((_received_msg_stats[stat_slot].msg_id == message.msgid)
			    && (_received_msg_stats[stat_slot].system_id == message.sysid)
			    && (_received_msg_stats[stat_slot].component_id == message.compid)) {

				msg_stats_slot = stat_slot;
				break;
			}
		}

		// otherwise find oldest or empty slot
		if (msg_stats_slot < 0) {
			uint32_t oldest_slot_time_ms = 0;

			for (int stat_slot = 0; stat_slot < MAX_MSG_STAT_SLOTS; stat_slot++) {
				if (_received_msg_stats[stat_slot].last_time_received_ms <= oldest_slot_time_ms) {
					oldest_slot_time_ms = _received_msg_stats[stat_slot].last_time_received_ms;
					msg_stats_slot = stat_slot;
				}
			}

			reset_stats = true;
		}

		if (msg_stats_slot >= 0) {
			if (!reset_stats) {
				if ((_received_msg_stats[msg_stats_slot].last_time_received_ms != 0)
				    && (now_ms > _received_msg_stats[msg_stats_slot].last_time_received_ms)) {

					float rate = 1000.f / (now_ms - _received_msg_stats[msg_stats_slot].last_time_received_ms);

					if (PX4_ISFINITE(_received_msg_stats[msg_stats_slot].avg_rate_hz)) {
						_received_msg_stats[msg_stats_slot].avg_rate_hz = 0.9f * _received_msg_stats[msg_stats_slot].avg_rate_hz + 0.1f * rate;

					} else {
						_received_msg_stats[msg_stats_slot].avg_rate_hz = rate;
					}

				} else {
					_received_msg_stats[msg_stats_slot].avg_rate_hz = 0.f;
				}

			} else {
				_received_msg_stats[msg_stats_slot].avg_rate_hz = NAN;
			}

			_received_msg_stats[msg_stats_slot].last_time_received_ms = now_ms;
			_received_msg_stats[msg_stats_slot].msg_id = message.msgid;
			_received_msg_stats[msg_stats_slot].system_id = message.sysid;
			_received_msg_stats[msg_stats_slot].component_id = message.compid;
		}
	}

#endif // !CONSTRAINED_FLASH
}