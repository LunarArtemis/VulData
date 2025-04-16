void
MavlinkReceiver::handle_message_gimbal_device_information(mavlink_message_t *msg)
{

	mavlink_gimbal_device_information_t gimbal_device_info_msg;
	mavlink_msg_gimbal_device_information_decode(msg, &gimbal_device_info_msg);

	gimbal_device_information_s gimbal_information{};
	gimbal_information.timestamp = hrt_absolute_time();

	static_assert(sizeof(gimbal_information.vendor_name) == sizeof(gimbal_device_info_msg.vendor_name),
		      "vendor_name length doesn't match");
	static_assert(sizeof(gimbal_information.model_name) == sizeof(gimbal_device_info_msg.model_name),
		      "model_name length doesn't match");
	static_assert(sizeof(gimbal_information.custom_name) == sizeof(gimbal_device_info_msg.custom_name),
		      "custom_name length doesn't match");
	memcpy(gimbal_information.vendor_name, gimbal_device_info_msg.vendor_name, sizeof(gimbal_information.vendor_name));
	memcpy(gimbal_information.model_name, gimbal_device_info_msg.model_name, sizeof(gimbal_information.model_name));
	memcpy(gimbal_information.custom_name, gimbal_device_info_msg.custom_name, sizeof(gimbal_information.custom_name));
	gimbal_device_info_msg.vendor_name[sizeof(gimbal_device_info_msg.vendor_name) - 1] = '\0';
	gimbal_device_info_msg.model_name[sizeof(gimbal_device_info_msg.model_name) - 1] = '\0';
	gimbal_device_info_msg.custom_name[sizeof(gimbal_device_info_msg.custom_name) - 1] = '\0';

	gimbal_information.firmware_version = gimbal_device_info_msg.firmware_version;
	gimbal_information.hardware_version = gimbal_device_info_msg.hardware_version;
	gimbal_information.cap_flags = gimbal_device_info_msg.cap_flags;
	gimbal_information.custom_cap_flags = gimbal_device_info_msg.custom_cap_flags;
	gimbal_information.uid = gimbal_device_info_msg.uid;

	gimbal_information.pitch_max = gimbal_device_info_msg.pitch_max;
	gimbal_information.pitch_min = gimbal_device_info_msg.pitch_min;

	gimbal_information.yaw_max = gimbal_device_info_msg.yaw_max;
	gimbal_information.yaw_min = gimbal_device_info_msg.yaw_min;

	gimbal_information.gimbal_device_compid = msg->compid;

	_gimbal_device_information_pub.publish(gimbal_information);
}