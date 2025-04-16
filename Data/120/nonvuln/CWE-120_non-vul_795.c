void
MavlinkReceiver::handle_message_utm_global_position(mavlink_message_t *msg)
{
	mavlink_utm_global_position_t utm_pos;
	mavlink_msg_utm_global_position_decode(msg, &utm_pos);

	bool is_self_published = false;


#ifndef BOARD_HAS_NO_UUID
	px4_guid_t px4_guid;
	board_get_px4_guid(px4_guid);
	is_self_published = sizeof(px4_guid) == sizeof(utm_pos.uas_id)
			    && memcmp(px4_guid, utm_pos.uas_id, sizeof(px4_guid_t)) == 0;
#else

	is_self_published = msg->sysid == _mavlink->get_system_id();
#endif /* BOARD_HAS_NO_UUID */


	//Ignore selfpublished UTM messages
	if (is_self_published) {
		return;
	}

	// Convert cm/s to m/s
	float vx = utm_pos.vx / 100.0f;
	float vy = utm_pos.vy / 100.0f;
	float vz = utm_pos.vz / 100.0f;

	transponder_report_s t{};
	t.timestamp = hrt_absolute_time();
	mav_array_memcpy(t.uas_id, utm_pos.uas_id, PX4_GUID_BYTE_LENGTH);
	t.lat = utm_pos.lat * 1e-7;
	t.lon = utm_pos.lon * 1e-7;
	t.altitude = utm_pos.alt / 1000.0f;
	t.altitude_type = ADSB_ALTITUDE_TYPE_GEOMETRIC;
	// UTM_GLOBAL_POSIION uses NED (north, east, down) coordinates for velocity, in cm / s.
	t.heading = atan2f(vy, vx);
	t.hor_velocity = sqrtf(vy * vy + vx * vx);
	t.ver_velocity = -vz;
	// TODO: Callsign
	// For now, set it to all 0s. This is a null-terminated string, so not explicitly giving it a null
	// terminator could cause problems.
	memset(&t.callsign[0], 0, sizeof(t.callsign));
	t.emitter_type = ADSB_EMITTER_TYPE_UAV;  // TODO: Is this correct?x2?

	// The Mavlink docs do not specify what to do if tslc (time since last communication) is out of range of
	// an 8-bit int, or if this is the first communication.
	// Here, I assume that if this is the first communication, tslc = 0.
	// If tslc > 255, then tslc = 255.
	unsigned long time_passed = (t.timestamp - _last_utm_global_pos_com) / 1000000;

	if (_last_utm_global_pos_com == 0) {
		time_passed = 0;

	} else if (time_passed > UINT8_MAX) {
		time_passed = UINT8_MAX;
	}

	t.tslc = (uint8_t) time_passed;

	t.flags = 0;

	if (utm_pos.flags & UTM_DATA_AVAIL_FLAGS_POSITION_AVAILABLE) {
		t.flags |= transponder_report_s::PX4_ADSB_FLAGS_VALID_COORDS;
	}

	if (utm_pos.flags & UTM_DATA_AVAIL_FLAGS_ALTITUDE_AVAILABLE) {
		t.flags |= transponder_report_s::PX4_ADSB_FLAGS_VALID_ALTITUDE;
	}

	if (utm_pos.flags & UTM_DATA_AVAIL_FLAGS_HORIZONTAL_VELO_AVAILABLE) {
		t.flags |= transponder_report_s::PX4_ADSB_FLAGS_VALID_HEADING;
		t.flags |= transponder_report_s::PX4_ADSB_FLAGS_VALID_VELOCITY;
	}

	// Note: t.flags has deliberately NOT set VALID_CALLSIGN or VALID_SQUAWK, because UTM_GLOBAL_POSITION does not
	// provide these.
	_transponder_report_pub.publish(t);

	_last_utm_global_pos_com = t.timestamp;
}