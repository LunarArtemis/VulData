void
MavlinkReceiver::handle_message_hil_optical_flow(mavlink_message_t *msg)
{
	/* optical flow */
	mavlink_hil_optical_flow_t flow;
	mavlink_msg_hil_optical_flow_decode(msg, &flow);

	optical_flow_s f{};

	f.timestamp = hrt_absolute_time(); // XXX we rely on the system time for now and not flow.time_usec;
	f.integration_timespan = flow.integration_time_us;
	f.pixel_flow_x_integral = flow.integrated_x;
	f.pixel_flow_y_integral = flow.integrated_y;
	f.gyro_x_rate_integral = flow.integrated_xgyro;
	f.gyro_y_rate_integral = flow.integrated_ygyro;
	f.gyro_z_rate_integral = flow.integrated_zgyro;
	f.time_since_last_sonar_update = flow.time_delta_distance_us;
	f.ground_distance_m = flow.distance;
	f.quality = flow.quality;
	f.sensor_id = flow.sensor_id;
	f.gyro_temperature = flow.temperature;

	_flow_pub.publish(f);

	/* Use distance value for distance sensor topic */
	distance_sensor_s d{};

	device::Device::DeviceId device_id;
	device_id.devid_s.bus = device::Device::DeviceBusType::DeviceBusType_MAVLINK;
	device_id.devid_s.devtype = DRV_DIST_DEVTYPE_MAVLINK;
	device_id.devid_s.address = msg->sysid;

	d.timestamp = hrt_absolute_time();
	d.min_distance = 0.3f;
	d.max_distance = 5.0f;
	d.current_distance = flow.distance; /* both are in m */
	d.type = distance_sensor_s::MAV_DISTANCE_SENSOR_LASER;
	d.device_id = device_id.devid;
	d.orientation = distance_sensor_s::ROTATION_DOWNWARD_FACING;
	d.variance = 0.0;

	_flow_distance_sensor_pub.publish(d);
}