void
MavlinkReceiver::handle_message_cellular_status(mavlink_message_t *msg)
{
	mavlink_cellular_status_t status;
	mavlink_msg_cellular_status_decode(msg, &status);

	cellular_status_s cellular_status{};

	cellular_status.timestamp = hrt_absolute_time();
	cellular_status.status = status.status;
	cellular_status.failure_reason = status.failure_reason;
	cellular_status.type = status.type;
	cellular_status.quality = status.quality;
	cellular_status.mcc = status.mcc;
	cellular_status.mnc = status.mnc;
	cellular_status.lac = status.lac;

	_cellular_status_pub.publish(cellular_status);
}