void
MavlinkReceiver::handle_message_landing_target(mavlink_message_t *msg)
{
	mavlink_landing_target_t landing_target;
	mavlink_msg_landing_target_decode(msg, &landing_target);

	if (landing_target.position_valid && landing_target.frame == MAV_FRAME_LOCAL_NED) {
		landing_target_pose_s landing_target_pose{};

		landing_target_pose.timestamp = _mavlink_timesync.sync_stamp(landing_target.time_usec);
		landing_target_pose.abs_pos_valid = true;
		landing_target_pose.x_abs = landing_target.x;
		landing_target_pose.y_abs = landing_target.y;
		landing_target_pose.z_abs = landing_target.z;

		_landing_target_pose_pub.publish(landing_target_pose);

	} else if (landing_target.position_valid) {
		// We only support MAV_FRAME_LOCAL_NED. In this case, the frame was unsupported.
		mavlink_log_critical(&_mavlink_log_pub, "landing target: coordinate frame %" PRIu8 " unsupported\t",
				     landing_target.frame);
		events::send<uint8_t>(events::ID("mavlink_rcv_lnd_target_unsup_coord"), events::Log::Error,
				      "landing target: unsupported coordinate frame {1}", landing_target.frame);

	} else {
		irlock_report_s irlock_report{};

		irlock_report.timestamp = hrt_absolute_time();
		irlock_report.signature = landing_target.target_num;
		irlock_report.pos_x = landing_target.angle_x;
		irlock_report.pos_y = landing_target.angle_y;
		irlock_report.size_x = landing_target.size_x;
		irlock_report.size_y = landing_target.size_y;

		_irlock_report_pub.publish(irlock_report);
	}
}