void MavlinkReceiver::CheckHeartbeats(const hrt_abstime &t, bool force)
{
	// check HEARTBEATs for timeout
	static constexpr uint64_t TIMEOUT = telemetry_status_s::HEARTBEAT_TIMEOUT_US;

	if (t <= TIMEOUT) {
		return;
	}

	if ((t >= _last_heartbeat_check + (TIMEOUT / 2)) || force) {
		telemetry_status_s &tstatus = _mavlink->telemetry_status();

		tstatus.heartbeat_type_antenna_tracker         = (t <= TIMEOUT + _heartbeat_type_antenna_tracker);
		tstatus.heartbeat_type_gcs                     = (t <= TIMEOUT + _heartbeat_type_gcs);
		tstatus.heartbeat_type_onboard_controller      = (t <= TIMEOUT + _heartbeat_type_onboard_controller);
		tstatus.heartbeat_type_gimbal                  = (t <= TIMEOUT + _heartbeat_type_gimbal);
		tstatus.heartbeat_type_adsb                    = (t <= TIMEOUT + _heartbeat_type_adsb);
		tstatus.heartbeat_type_camera                  = (t <= TIMEOUT + _heartbeat_type_camera);

		tstatus.heartbeat_component_telemetry_radio    = (t <= TIMEOUT + _heartbeat_component_telemetry_radio);
		tstatus.heartbeat_component_log                = (t <= TIMEOUT + _heartbeat_component_log);
		tstatus.heartbeat_component_osd                = (t <= TIMEOUT + _heartbeat_component_osd);
		tstatus.heartbeat_component_obstacle_avoidance = (t <= TIMEOUT + _heartbeat_component_obstacle_avoidance);
		tstatus.heartbeat_component_vio                = (t <= TIMEOUT + _heartbeat_component_visual_inertial_odometry);
		tstatus.heartbeat_component_pairing_manager    = (t <= TIMEOUT + _heartbeat_component_pairing_manager);
		tstatus.heartbeat_component_udp_bridge         = (t <= TIMEOUT + _heartbeat_component_udp_bridge);
		tstatus.heartbeat_component_uart_bridge        = (t <= TIMEOUT + _heartbeat_component_uart_bridge);

		_mavlink->telemetry_status_updated();
		_last_heartbeat_check = t;
	}
}