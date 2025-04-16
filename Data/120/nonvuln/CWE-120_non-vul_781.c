void
MavlinkReceiver::handle_message_heartbeat(mavlink_message_t *msg)
{
	/* telemetry status supported only on first TELEMETRY_STATUS_ORB_ID_NUM mavlink channels */
	if (_mavlink->get_channel() < (mavlink_channel_t)ORB_MULTI_MAX_INSTANCES) {

		const hrt_abstime now = hrt_absolute_time();

		mavlink_heartbeat_t hb;
		mavlink_msg_heartbeat_decode(msg, &hb);

		const bool same_system = (msg->sysid == mavlink_system.sysid);

		if (same_system || hb.type == MAV_TYPE_GCS) {

			camera_status_s camera_status{};

			switch (hb.type) {
			case MAV_TYPE_ANTENNA_TRACKER:
				_heartbeat_type_antenna_tracker = now;
				break;

			case MAV_TYPE_GCS:
				_heartbeat_type_gcs = now;
				break;

			case MAV_TYPE_ONBOARD_CONTROLLER:
				_heartbeat_type_onboard_controller = now;
				break;

			case MAV_TYPE_GIMBAL:
				_heartbeat_type_gimbal = now;
				break;

			case MAV_TYPE_ADSB:
				_heartbeat_type_adsb = now;
				break;

			case MAV_TYPE_CAMERA:
				_heartbeat_type_camera = now;
				camera_status.timestamp = now;
				camera_status.active_comp_id = msg->compid;
				camera_status.active_sys_id = msg->sysid;
				_camera_status_pub.publish(camera_status);
				break;

			default:
				PX4_DEBUG("unhandled HEARTBEAT MAV_TYPE: %" PRIu8 " from SYSID: %" PRIu8 ", COMPID: %" PRIu8, hb.type, msg->sysid,
					  msg->compid);
			}


			switch (msg->compid) {
			case MAV_COMP_ID_TELEMETRY_RADIO:
				_heartbeat_component_telemetry_radio = now;
				break;

			case MAV_COMP_ID_LOG:
				_heartbeat_component_log = now;
				break;

			case MAV_COMP_ID_OSD:
				_heartbeat_component_osd = now;
				break;

			case MAV_COMP_ID_OBSTACLE_AVOIDANCE:
				_heartbeat_component_obstacle_avoidance = now;
				_mavlink->telemetry_status().avoidance_system_healthy = (hb.system_status == MAV_STATE_ACTIVE);
				break;

			case MAV_COMP_ID_VISUAL_INERTIAL_ODOMETRY:
				_heartbeat_component_visual_inertial_odometry = now;
				break;

			case MAV_COMP_ID_PAIRING_MANAGER:
				_heartbeat_component_pairing_manager = now;
				break;

			case MAV_COMP_ID_UDP_BRIDGE:
				_heartbeat_component_udp_bridge = now;
				break;

			case MAV_COMP_ID_UART_BRIDGE:
				_heartbeat_component_uart_bridge = now;
				break;

			default:
				PX4_DEBUG("unhandled HEARTBEAT MAV_TYPE: %" PRIu8 " from SYSID: %" PRIu8 ", COMPID: %" PRIu8, hb.type, msg->sysid,
					  msg->compid);
			}

			CheckHeartbeats(now, true);
		}
	}
}