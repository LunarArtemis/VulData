void MavlinkReceiver::print_detailed_rx_stats() const
{
	// TODO: add mutex around shared data.
	if (_component_states_count > 0) {
		printf("\tReceived Messages:\n");

		for (const auto &comp_stat : _component_states) {
			if (comp_stat.received_messages > 0) {
				printf("\t  sysid:%3" PRIu8 ", compid:%3" PRIu8 ", Total: %" PRIu32 " (lost: %" PRIu32 ")\n",
				       comp_stat.system_id, comp_stat.component_id,
				       comp_stat.received_messages, comp_stat.missed_messages);

#if !defined(CONSTRAINED_FLASH)

				if (_message_statistics_enabled && _received_msg_stats) {
					for (int i = 0; i < MAX_MSG_STAT_SLOTS; i++) {
						const ReceivedMessageStats &msg_stat = _received_msg_stats[i];

						const uint32_t now_ms = hrt_absolute_time() / 1000;

						// valid messages received within the last 10 seconds
						if ((msg_stat.system_id == comp_stat.system_id)
						    && (msg_stat.component_id == comp_stat.component_id)
						    && (msg_stat.last_time_received_ms != 0)
						    && (now_ms - msg_stat.last_time_received_ms < 10'000)) {

							const float elapsed_s = (now_ms - msg_stat.last_time_received_ms) / 1000.f;

							printf("\t    msgid:%5" PRIu16 ", Rate:%5.1f Hz, last %.2fs ago\n",
							       msg_stat.msg_id, (double)msg_stat.avg_rate_hz, (double)elapsed_s);
						}
					}
				}

#endif // !CONSTRAINED_FLASH
			}
		}
	}
}