void
MavlinkReceiver::handle_message_trajectory_representation_bezier(mavlink_message_t *msg)
{
	mavlink_trajectory_representation_bezier_t trajectory;
	mavlink_msg_trajectory_representation_bezier_decode(msg, &trajectory);

	vehicle_trajectory_bezier_s trajectory_bezier{};

	trajectory_bezier.timestamp =  _mavlink_timesync.sync_stamp(trajectory.time_usec);

	for (int i = 0; i < vehicle_trajectory_bezier_s::NUMBER_POINTS; ++i) {
		trajectory_bezier.control_points[i].position[0] = trajectory.pos_x[i];
		trajectory_bezier.control_points[i].position[1] = trajectory.pos_y[i];
		trajectory_bezier.control_points[i].position[2] = trajectory.pos_z[i];

		trajectory_bezier.control_points[i].delta = trajectory.delta[i];
		trajectory_bezier.control_points[i].yaw = trajectory.pos_yaw[i];
	}

	trajectory_bezier.bezier_order = math::min(trajectory.valid_points, vehicle_trajectory_bezier_s::NUMBER_POINTS);
	_trajectory_bezier_pub.publish(trajectory_bezier);
}