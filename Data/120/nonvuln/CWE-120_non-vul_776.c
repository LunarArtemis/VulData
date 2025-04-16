void
MavlinkReceiver::handle_message_distance_sensor(mavlink_message_t *msg)
{
	mavlink_distance_sensor_t dist_sensor;
	mavlink_msg_distance_sensor_decode(msg, &dist_sensor);

	distance_sensor_s ds{};

	device::Device::DeviceId device_id;
	device_id.devid_s.bus = device::Device::DeviceBusType::DeviceBusType_MAVLINK;
	device_id.devid_s.devtype = DRV_DIST_DEVTYPE_MAVLINK;
	device_id.devid_s.address = dist_sensor.id;

	ds.timestamp        = hrt_absolute_time(); /* Use system time for now, don't trust sender to attach correct timestamp */
	ds.min_distance     = static_cast<float>(dist_sensor.min_distance) * 1e-2f;     /* cm to m */
	ds.max_distance     = static_cast<float>(dist_sensor.max_distance) * 1e-2f;     /* cm to m */
	ds.current_distance = static_cast<float>(dist_sensor.current_distance) * 1e-2f; /* cm to m */
	ds.variance         = dist_sensor.covariance * 1e-4f;                           /* cm^2 to m^2 */
	ds.h_fov            = dist_sensor.horizontal_fov;
	ds.v_fov            = dist_sensor.vertical_fov;
	ds.q[0]             = dist_sensor.quaternion[0];
	ds.q[1]             = dist_sensor.quaternion[1];
	ds.q[2]             = dist_sensor.quaternion[2];
	ds.q[3]             = dist_sensor.quaternion[3];
	ds.type             = dist_sensor.type;
	ds.device_id        = device_id.devid;
	ds.orientation      = dist_sensor.orientation;

	// MAVLink DISTANCE_SENSOR signal_quality value of 0 means unset/unknown
	// quality value. Also it comes normalised between 1 and 100 while the uORB
	// signal quality is normalised between 0 and 100.
	ds.signal_quality = dist_sensor.signal_quality == 0 ? -1 : 100 * (dist_sensor.signal_quality - 1) / 99;

	_distance_sensor_pub.publish(ds);
}