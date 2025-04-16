void
MavlinkReceiver::handle_message_obstacle_distance(mavlink_message_t *msg)
{
	mavlink_obstacle_distance_t mavlink_obstacle_distance;
	mavlink_msg_obstacle_distance_decode(msg, &mavlink_obstacle_distance);

	obstacle_distance_s obstacle_distance{};

	obstacle_distance.timestamp = hrt_absolute_time();
	obstacle_distance.sensor_type = mavlink_obstacle_distance.sensor_type;
	memcpy(obstacle_distance.distances, mavlink_obstacle_distance.distances, sizeof(obstacle_distance.distances));

	if (mavlink_obstacle_distance.increment_f > 0.f) {
		obstacle_distance.increment = mavlink_obstacle_distance.increment_f;

	} else {
		obstacle_distance.increment = (float)mavlink_obstacle_distance.increment;
	}

	obstacle_distance.min_distance = mavlink_obstacle_distance.min_distance;
	obstacle_distance.max_distance = mavlink_obstacle_distance.max_distance;
	obstacle_distance.angle_offset = mavlink_obstacle_distance.angle_offset;
	obstacle_distance.frame = mavlink_obstacle_distance.frame;

	_obstacle_distance_pub.publish(obstacle_distance);
}