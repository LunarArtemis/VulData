void
MavlinkReceiver::handle_message_vision_position_estimate(mavlink_message_t *msg)
{
	mavlink_vision_position_estimate_t ev;
	mavlink_msg_vision_position_estimate_decode(msg, &ev);

	vehicle_odometry_s visual_odom{};

	visual_odom.timestamp = hrt_absolute_time();
	visual_odom.timestamp_sample = _mavlink_timesync.sync_stamp(ev.usec);

	visual_odom.x = ev.x;
	visual_odom.y = ev.y;
	visual_odom.z = ev.z;
	matrix::Quatf q(matrix::Eulerf(ev.roll, ev.pitch, ev.yaw));
	q.copyTo(visual_odom.q);

	visual_odom.local_frame = vehicle_odometry_s::LOCAL_FRAME_NED;

	const size_t URT_SIZE = sizeof(visual_odom.pose_covariance) / sizeof(visual_odom.pose_covariance[0]);
	static_assert(URT_SIZE == (sizeof(ev.covariance) / sizeof(ev.covariance[0])),
		      "Odometry Pose Covariance matrix URT array size mismatch");

	for (size_t i = 0; i < URT_SIZE; i++) {
		visual_odom.pose_covariance[i] = ev.covariance[i];
	}

	visual_odom.velocity_frame = vehicle_odometry_s::LOCAL_FRAME_FRD;
	visual_odom.vx = NAN;
	visual_odom.vy = NAN;
	visual_odom.vz = NAN;
	visual_odom.rollspeed = NAN;
	visual_odom.pitchspeed = NAN;
	visual_odom.yawspeed = NAN;
	visual_odom.velocity_covariance[0] = NAN;

	_visual_odometry_pub.publish(visual_odom);
}