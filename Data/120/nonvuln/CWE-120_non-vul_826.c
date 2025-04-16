void
MavlinkReceiver::handle_message_gimbal_manager_set_attitude(mavlink_message_t *msg)
{
	mavlink_gimbal_manager_set_attitude_t set_attitude_msg;
	mavlink_msg_gimbal_manager_set_attitude_decode(msg, &set_attitude_msg);

	gimbal_manager_set_attitude_s gimbal_attitude{};
	gimbal_attitude.timestamp = hrt_absolute_time();
	gimbal_attitude.origin_sysid = msg->sysid;
	gimbal_attitude.origin_compid = msg->compid;
	gimbal_attitude.target_system = set_attitude_msg.target_system;
	gimbal_attitude.target_component = set_attitude_msg.target_component;
	gimbal_attitude.flags = set_attitude_msg.flags;
	gimbal_attitude.gimbal_device_id = set_attitude_msg.gimbal_device_id;

	matrix::Quatf q(set_attitude_msg.q);
	q.copyTo(gimbal_attitude.q);

	gimbal_attitude.angular_velocity_x = set_attitude_msg.angular_velocity_x;
	gimbal_attitude.angular_velocity_y = set_attitude_msg.angular_velocity_y;
	gimbal_attitude.angular_velocity_z = set_attitude_msg.angular_velocity_z;

	_gimbal_manager_set_attitude_pub.publish(gimbal_attitude);
}