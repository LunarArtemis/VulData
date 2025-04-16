void
MavlinkReceiver::handle_message_optical_flow_rad(mavlink_message_t *msg)
{
	/* optical flow */
	mavlink_optical_flow_rad_t flow;
	mavlink_msg_optical_flow_rad_decode(msg, &flow);

	optical_flow_s f{};

	f.timestamp = hrt_absolute_time();
	f.time_since_last_sonar_update = flow.time_delta_distance_us;
	f.integration_timespan  = flow.integration_time_us;
	f.pixel_flow_x_integral = flow.integrated_x;
	f.pixel_flow_y_integral = flow.integrated_y;
	f.gyro_x_rate_integral  = flow.integrated_xgyro;
	f.gyro_y_rate_integral  = flow.integrated_ygyro;
	f.gyro_z_rate_integral  = flow.integrated_zgyro;
	f.gyro_temperature      = flow.temperature;
	f.ground_distance_m     = flow.distance;
	f.quality               = flow.quality;
	f.sensor_id             = flow.sensor_id;
	f.max_flow_rate         = _param_sens_flow_maxr;
	f.min_ground_distance   = _param_sens_flow_minhgt;
	f.max_ground_distance   = _param_sens_flow_maxhgt;

	/* read flow sensor parameters */
	const Rotation flow_rot = (Rotation)_param_sens_flow_rot;

	/* rotate measurements according to parameter */
	float zero_val = 0.0f;
	rotate_3f(flow_rot, f.pixel_flow_x_integral, f.pixel_flow_y_integral, zero_val);
	rotate_3f(flow_rot, f.gyro_x_rate_integral, f.gyro_y_rate_integral, f.gyro_z_rate_integral);

	_flow_pub.publish(f);

	/* Use distance value for distance sensor topic */
	if (flow.distance > 0.0f) { // negative values signal invalid data

		distance_sensor_s d{};

		device::Device::DeviceId device_id;
		device_id.devid_s.bus = device::Device::DeviceBusType::DeviceBusType_MAVLINK;
		device_id.devid_s.devtype = DRV_DIST_DEVTYPE_MAVLINK;
		device_id.devid_s.address = msg->sysid;

		d.timestamp = f.timestamp;
		d.min_distance = 0.3f;
		d.max_distance = 5.0f;
		d.current_distance = flow.distance; /* both are in m */
		d.type = distance_sensor_s::MAV_DISTANCE_SENSOR_ULTRASOUND;
		d.device_id = device_id.devid;
		d.orientation = distance_sensor_s::ROTATION_DOWNWARD_FACING;
		d.variance = 0.0;

		_flow_distance_sensor_pub.publish(d);
	}
}