void
MavlinkReceiver::handle_message_ping(mavlink_message_t *msg)
{
	mavlink_ping_t ping;
	mavlink_msg_ping_decode(msg, &ping);

	if ((ping.target_system == 0) &&
	    (ping.target_component == 0)) {	   // This is a ping request. Return it to the system which requested the ping.

		ping.target_system = msg->sysid;
		ping.target_component = msg->compid;
		mavlink_msg_ping_send_struct(_mavlink->get_channel(), &ping);

	} else if ((ping.target_system == mavlink_system.sysid) &&
		   (ping.target_component ==
		    mavlink_system.compid)) { // This is a returned ping message from this system. Calculate latency from it.

		const hrt_abstime now = hrt_absolute_time();

		// Calculate round trip time
		float rtt_ms = (now - ping.time_usec) / 1000.0f;

		// Update ping statistics
		struct Mavlink::ping_statistics_s &pstats = _mavlink->get_ping_statistics();

		pstats.last_ping_time = now;

		if (pstats.last_ping_seq == 0 && ping.seq > 0) {
			// This is the first reply we are receiving from an offboard system.
			// We may have been broadcasting pings for some time before it came online,
			// and these do not count as dropped packets.

			// Reset last_ping_seq counter for correct packet drop detection
			pstats.last_ping_seq = ping.seq - 1;
		}

		// We can only count dropped packets after the first message
		if (ping.seq > pstats.last_ping_seq) {
			pstats.dropped_packets += ping.seq - pstats.last_ping_seq - 1;
		}

		pstats.last_ping_seq = ping.seq;
		pstats.last_rtt = rtt_ms;
		pstats.mean_rtt = (rtt_ms + pstats.mean_rtt) / 2.0f;
		pstats.max_rtt = fmaxf(rtt_ms, pstats.max_rtt);
		pstats.min_rtt = pstats.min_rtt > 0.0f ? fminf(rtt_ms, pstats.min_rtt) : rtt_ms;

		/* Ping status is supported only on first ORB_MULTI_MAX_INSTANCES mavlink channels */
		if (_mavlink->get_channel() < (mavlink_channel_t)ORB_MULTI_MAX_INSTANCES) {

			ping_s uorb_ping_msg{};

			uorb_ping_msg.timestamp = now;
			uorb_ping_msg.ping_time = ping.time_usec;
			uorb_ping_msg.ping_sequence = ping.seq;
			uorb_ping_msg.dropped_packets = pstats.dropped_packets;
			uorb_ping_msg.rtt_ms = rtt_ms;
			uorb_ping_msg.system_id = msg->sysid;
			uorb_ping_msg.component_id = msg->compid;

			_ping_pub.publish(uorb_ping_msg);
		}
	}
}