void MavlinkReceiver::handle_message_generator_status(mavlink_message_t *msg)
{
	mavlink_generator_status_t status_msg;
	mavlink_msg_generator_status_decode(msg, &status_msg);

	generator_status_s generator_status{};
	generator_status.timestamp = hrt_absolute_time();
	generator_status.status = status_msg.status;
	generator_status.battery_current = status_msg.battery_current;
	generator_status.load_current = status_msg.load_current;
	generator_status.power_generated = status_msg.power_generated;
	generator_status.bus_voltage = status_msg.bus_voltage;
	generator_status.bat_current_setpoint = status_msg.bat_current_setpoint;
	generator_status.runtime = status_msg.runtime;
	generator_status.time_until_maintenance = status_msg.time_until_maintenance;
	generator_status.generator_speed = status_msg.generator_speed;
	generator_status.rectifier_temperature = status_msg.rectifier_temperature;
	generator_status.generator_temperature = status_msg.generator_temperature;

	_generator_status_pub.publish(generator_status);
}