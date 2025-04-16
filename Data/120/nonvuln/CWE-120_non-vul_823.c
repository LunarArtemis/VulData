void MavlinkReceiver::update_rx_stats(const mavlink_message_t &message)
{
	const bool component_states_has_still_space = [this, &message]() {
		for (unsigned i = 0; i < MAX_REMOTE_COMPONENTS; ++i) {
			if (_component_states[i].system_id == message.sysid && _component_states[i].component_id == message.compid) {

				int lost_messages = 0;
				const uint8_t expected_seq = _component_states[i].last_sequence + 1;

				// Account for overflow during packet loss
				if (message.seq < expected_seq) {
					lost_messages = (message.seq + 255) - expected_seq;

				} else {
					lost_messages = message.seq - expected_seq;
				}

				_component_states[i].missed_messages += lost_messages;

				++_component_states[i].received_messages;
				_component_states[i].last_sequence = message.seq;

				// Also update overall stats
				++_total_received_counter;
				_total_lost_counter += lost_messages;

				return true;

			} else if (_component_states[i].system_id == 0 && _component_states[i].component_id == 0) {
				_component_states[i].system_id = message.sysid;
				_component_states[i].component_id = message.compid;

				++_component_states[i].received_messages;
				_component_states[i].last_sequence = message.seq;

				_component_states_count = i + 1;

				// Also update overall stats
				++_total_received_counter;

				return true;
			}
		}

		return false;
	}();

	if (!component_states_has_still_space && !_warned_component_states_full_once) {
		PX4_WARN("Max remote components of %u used up", MAX_REMOTE_COMPONENTS);
		_warned_component_states_full_once = true;
	}
}