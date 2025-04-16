void
MavlinkReceiver::handle_message_trajectory_representation_waypoints(mavlink_message_t *msg)
{
	mavlink_trajectory_representation_waypoints_t trajectory;
	mavlink_msg_trajectory_representation_waypoints_decode(msg, &trajectory);

	vehicle_trajectory_waypoint_s trajectory_waypoint{};

	trajectory_waypoint.timestamp = hrt_absolute_time();
	const int number_valid_points = trajectory.valid_points;

	for (int i = 0; i < vehicle_trajectory_waypoint_s::NUMBER_POINTS; ++i) {
		trajectory_waypoint.waypoints[i].position[0] = trajectory.pos_x[i];
		trajectory_waypoint.waypoints[i].position[1] = trajectory.pos_y[i];
		trajectory_waypoint.waypoints[i].position[2] = trajectory.pos_z[i];

		trajectory_waypoint.waypoints[i].velocity[0] = trajectory.vel_x[i];
		trajectory_waypoint.waypoints[i].velocity[1] = trajectory.vel_y[i];
		trajectory_waypoint.waypoints[i].velocity[2] = trajectory.vel_z[i];

		trajectory_waypoint.waypoints[i].acceleration[0] = trajectory.acc_x[i];
		trajectory_waypoint.waypoints[i].acceleration[1] = trajectory.acc_y[i];
		trajectory_waypoint.waypoints[i].acceleration[2] = trajectory.acc_z[i];

		trajectory_waypoint.waypoints[i].yaw = trajectory.pos_yaw[i];
		trajectory_waypoint.waypoints[i].yaw_speed = trajectory.vel_yaw[i];

		trajectory_waypoint.waypoints[i].type = UINT8_MAX;
	}

	for (int i = 0; i < number_valid_points; ++i) {
		trajectory_waypoint.waypoints[i].point_valid = true;
	}

	for (int i = number_valid_points; i < vehicle_trajectory_waypoint_s::NUMBER_POINTS; ++i) {
		trajectory_waypoint.waypoints[i].point_valid = false;
	}

	_trajectory_waypoint_pub.publish(trajectory_waypoint);
}