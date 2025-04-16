void
MavlinkReceiver::handle_message_odometry(mavlink_message_t *msg)
{
	mavlink_odometry_t odom;
	mavlink_msg_odometry_decode(msg, &odom);

	vehicle_odometry_s odometry{};

	odometry.timestamp = hrt_absolute_time();
	odometry.timestamp_sample = _mavlink_timesync.sync_stamp(odom.time_usec);

	/* The position is in a local FRD frame */
	odometry.x = odom.x;
	odometry.y = odom.y;
	odometry.z = odom.z;

	/**
	 * The quaternion of the ODOMETRY msg represents a rotation from body frame
	 * to a local frame
	 */
	matrix::Quatf q_body_to_local(odom.q);
	q_body_to_local.normalize();
	q_body_to_local.copyTo(odometry.q);

	// pose_covariance
	static constexpr size_t POS_URT_SIZE = sizeof(odometry.pose_covariance) / sizeof(odometry.pose_covariance[0]);
	static_assert(POS_URT_SIZE == (sizeof(odom.pose_covariance) / sizeof(odom.pose_covariance[0])),
		      "Odometry Pose Covariance matrix URT array size mismatch");

	// velocity_covariance
	static constexpr size_t VEL_URT_SIZE = sizeof(odometry.velocity_covariance) / sizeof(odometry.velocity_covariance[0]);
	static_assert(VEL_URT_SIZE == (sizeof(odom.velocity_covariance) / sizeof(odom.velocity_covariance[0])),
		      "Odometry Velocity Covariance matrix URT array size mismatch");

	// TODO: create a method to simplify covariance copy
	for (size_t i = 0; i < POS_URT_SIZE; i++) {
		odometry.pose_covariance[i] = odom.pose_covariance[i];
	}

	/**
	 * PX4 expects the body's linear velocity in the local frame,
	 * the linear velocity is rotated from the odom child_frame to the
	 * local NED frame. The angular velocity needs to be expressed in the
	 * body (fcu_frd) frame.
	 */
	if (odom.child_frame_id == MAV_FRAME_BODY_FRD) {

		odometry.velocity_frame = vehicle_odometry_s::BODY_FRAME_FRD;
		odometry.vx = odom.vx;
		odometry.vy = odom.vy;
		odometry.vz = odom.vz;

		odometry.rollspeed = odom.rollspeed;
		odometry.pitchspeed = odom.pitchspeed;
		odometry.yawspeed = odom.yawspeed;

		for (size_t i = 0; i < VEL_URT_SIZE; i++) {
			odometry.velocity_covariance[i] = odom.velocity_covariance[i];
		}

	} else {
		PX4_ERR("Body frame %" PRIu8 " not supported. Unable to publish velocity", odom.child_frame_id);
	}

	/**
	 * Supported local frame of reference is MAV_FRAME_LOCAL_NED or MAV_FRAME_LOCAL_FRD
	 * The supported sources of the data/tesimator type are MAV_ESTIMATOR_TYPE_VISION,
	 * MAV_ESTIMATOR_TYPE_VIO and MAV_ESTIMATOR_TYPE_MOCAP
	 *
	 * @note Regarding the local frames of reference, the appropriate EKF_AID_MASK
	 * should be set in order to match a frame aligned (NED) or not aligned (FRD)
	 * with true North
	 */
	if (odom.frame_id == MAV_FRAME_LOCAL_NED || odom.frame_id == MAV_FRAME_LOCAL_FRD) {

		if (odom.frame_id == MAV_FRAME_LOCAL_NED) {
			odometry.local_frame = vehicle_odometry_s::LOCAL_FRAME_NED;

		} else {
			odometry.local_frame = vehicle_odometry_s::LOCAL_FRAME_FRD;
		}

		if ((odom.estimator_type == MAV_ESTIMATOR_TYPE_VISION)
		    || (odom.estimator_type == MAV_ESTIMATOR_TYPE_VIO)
		    || (odom.estimator_type == MAV_ESTIMATOR_TYPE_UNKNOWN)) {
			// accept MAV_ESTIMATOR_TYPE_UNKNOWN for legacy support
			_visual_odometry_pub.publish(odometry);

		} else if (odom.estimator_type == MAV_ESTIMATOR_TYPE_MOCAP) {
			_mocap_odometry_pub.publish(odometry);

		} else {
			PX4_ERR("Estimator source %" PRIu8 " not supported. Unable to publish pose and velocity", odom.estimator_type);
		}

	} else {
		PX4_ERR("Local frame %" PRIu8 " not supported. Unable to publish pose and velocity", odom.frame_id);
	}
}