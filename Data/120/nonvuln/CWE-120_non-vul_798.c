void MavlinkReceiver::handle_message_statustext(mavlink_message_t *msg)
{
	if (msg->sysid == mavlink_system.sysid) {
		// log message from the same system

		mavlink_statustext_t statustext;
		mavlink_msg_statustext_decode(msg, &statustext);

		log_message_s log_message{};

		log_message.severity = statustext.severity;
		log_message.timestamp = hrt_absolute_time();

		snprintf(log_message.text, sizeof(log_message.text),
			 "[mavlink: component %" PRIu8 "] %." STRINGIFY(MAVLINK_MSG_STATUSTEXT_FIELD_TEXT_LEN) "s", msg->compid,
			 statustext.text);

		_log_message_pub.publish(log_message);
	}
}