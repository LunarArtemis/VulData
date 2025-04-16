void
MavlinkReceiver::handle_message_adsb_vehicle(mavlink_message_t *msg)
{
	mavlink_adsb_vehicle_t adsb;
	mavlink_msg_adsb_vehicle_decode(msg, &adsb);

	transponder_report_s t{};

	t.timestamp = hrt_absolute_time();

	t.icao_address = adsb.ICAO_address;
	t.lat = adsb.lat * 1e-7;
	t.lon = adsb.lon * 1e-7;
	t.altitude_type = adsb.altitude_type;
	t.altitude = adsb.altitude / 1000.0f;
	t.heading = adsb.heading / 100.0f / 180.0f * M_PI_F - M_PI_F;
	t.hor_velocity = adsb.hor_velocity / 100.0f;
	t.ver_velocity = adsb.ver_velocity / 100.0f;
	memcpy(&t.callsign[0], &adsb.callsign[0], sizeof(t.callsign));
	t.emitter_type = adsb.emitter_type;
	t.tslc = adsb.tslc;
	t.squawk = adsb.squawk;

	t.flags = transponder_report_s::PX4_ADSB_FLAGS_RETRANSLATE;  //Unset in receiver already broadcast its messages

	if (adsb.flags & ADSB_FLAGS_VALID_COORDS) { t.flags |= transponder_report_s::PX4_ADSB_FLAGS_VALID_COORDS; }

	if (adsb.flags & ADSB_FLAGS_VALID_ALTITUDE) { t.flags |= transponder_report_s::PX4_ADSB_FLAGS_VALID_ALTITUDE; }

	if (adsb.flags & ADSB_FLAGS_VALID_HEADING) { t.flags |= transponder_report_s::PX4_ADSB_FLAGS_VALID_HEADING; }

	if (adsb.flags & ADSB_FLAGS_VALID_VELOCITY) { t.flags |= transponder_report_s::PX4_ADSB_FLAGS_VALID_VELOCITY; }

	if (adsb.flags & ADSB_FLAGS_VALID_CALLSIGN) { t.flags |= transponder_report_s::PX4_ADSB_FLAGS_VALID_CALLSIGN; }

	if (adsb.flags & ADSB_FLAGS_VALID_SQUAWK) { t.flags |= transponder_report_s::PX4_ADSB_FLAGS_VALID_SQUAWK; }

	//PX4_INFO("code: %d callsign: %s, vel: %8.4f, tslc: %d", (int)t.ICAO_address, t.callsign, (double)t.hor_velocity, (int)t.tslc);

	_transponder_report_pub.publish(t);
}