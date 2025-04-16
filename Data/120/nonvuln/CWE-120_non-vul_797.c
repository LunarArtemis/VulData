void
MavlinkReceiver::handle_message_gimbal_manager_set_manual_control(mavlink_message_t *msg)
{
	mavlink_gimbal_manager_set_manual_control_t set_manual_control_msg;
	mavlink_msg_gimbal_manager_set_manual_control_decode(msg, &set_manual_control_msg);

	gimbal_manager_set_manual_control_s set_manual_control{};
	set_manual_control.timestamp = hrt_absolute_time();
	set_manual_control.origin_sysid = msg->sysid;
	set_manual_control.origin_compid = msg->compid;
	set_manual_control.target_system = set_manual_control_msg.target_system;
	set_manual_control.target_component = set_manual_control_msg.target_component;
	set_manual_control.flags = set_manual_control_msg.flags;
	set_manual_control.gimbal_device_id = set_manual_control_msg.gimbal_device_id;

	set_manual_control.pitch = set_manual_control_msg.pitch;
	set_manual_control.yaw = set_manual_control_msg.yaw;
	set_manual_control.pitch_rate = set_manual_control_msg.pitch_rate;
	set_manual_control.yaw_rate = set_manual_control_msg.yaw_rate;

	_gimbal_manager_set_manual_control_pub.publish(set_manual_control);
}