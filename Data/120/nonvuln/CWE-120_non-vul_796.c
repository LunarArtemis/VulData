void
MavlinkReceiver::handle_message_onboard_computer_status(mavlink_message_t *msg)
{
	mavlink_onboard_computer_status_t status_msg;
	mavlink_msg_onboard_computer_status_decode(msg, &status_msg);

	onboard_computer_status_s onboard_computer_status_topic{};

	onboard_computer_status_topic.timestamp = hrt_absolute_time();
	onboard_computer_status_topic.uptime = status_msg.uptime;

	onboard_computer_status_topic.type = status_msg.type;

	memcpy(onboard_computer_status_topic.cpu_cores, status_msg.cpu_cores, sizeof(status_msg.cpu_cores));
	memcpy(onboard_computer_status_topic.cpu_combined, status_msg.cpu_combined, sizeof(status_msg.cpu_combined));
	memcpy(onboard_computer_status_topic.gpu_cores, status_msg.gpu_cores, sizeof(status_msg.gpu_cores));
	memcpy(onboard_computer_status_topic.gpu_combined, status_msg.gpu_combined, sizeof(status_msg.gpu_combined));
	onboard_computer_status_topic.temperature_board = status_msg.temperature_board;
	memcpy(onboard_computer_status_topic.temperature_core, status_msg.temperature_core,
	       sizeof(status_msg.temperature_core));
	memcpy(onboard_computer_status_topic.fan_speed, status_msg.fan_speed, sizeof(status_msg.fan_speed));
	onboard_computer_status_topic.ram_usage = status_msg.ram_usage;
	onboard_computer_status_topic.ram_total = status_msg.ram_total;
	memcpy(onboard_computer_status_topic.storage_type, status_msg.storage_type, sizeof(status_msg.storage_type));
	memcpy(onboard_computer_status_topic.storage_usage, status_msg.storage_usage, sizeof(status_msg.storage_usage));
	memcpy(onboard_computer_status_topic.storage_total, status_msg.storage_total, sizeof(status_msg.storage_total));
	memcpy(onboard_computer_status_topic.link_type, status_msg.link_type, sizeof(status_msg.link_type));
	memcpy(onboard_computer_status_topic.link_tx_rate, status_msg.link_tx_rate, sizeof(status_msg.link_tx_rate));
	memcpy(onboard_computer_status_topic.link_rx_rate, status_msg.link_rx_rate, sizeof(status_msg.link_rx_rate));
	memcpy(onboard_computer_status_topic.link_tx_max, status_msg.link_tx_max, sizeof(status_msg.link_tx_max));
	memcpy(onboard_computer_status_topic.link_rx_max, status_msg.link_rx_max, sizeof(status_msg.link_rx_max));

	_onboard_computer_status_pub.publish(onboard_computer_status_topic);
}