void
MavlinkReceiver::handle_message_gps_rtcm_data(mavlink_message_t *msg)
{
	mavlink_gps_rtcm_data_t gps_rtcm_data_msg;
	mavlink_msg_gps_rtcm_data_decode(msg, &gps_rtcm_data_msg);

	gps_inject_data_s gps_inject_data_topic{};

	gps_inject_data_topic.len = math::min((int)sizeof(gps_rtcm_data_msg.data),
					      (int)sizeof(uint8_t) * gps_rtcm_data_msg.len);
	gps_inject_data_topic.flags = gps_rtcm_data_msg.flags;
	memcpy(gps_inject_data_topic.data, gps_rtcm_data_msg.data,
	       math::min((int)sizeof(gps_inject_data_topic.data), (int)sizeof(uint8_t) * gps_inject_data_topic.len));

	_gps_inject_data_pub.publish(gps_inject_data_topic);
}