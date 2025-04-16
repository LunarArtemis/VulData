MavlinkReceiver::MavlinkReceiver(Mavlink *parent) :
	ModuleParams(nullptr),
	_mavlink(parent),
	_mavlink_ftp(parent),
	_mavlink_log_handler(parent),
	_mission_manager(parent),
	_parameters_manager(parent),
	_mavlink_timesync(parent)
{
	_handle_sens_flow_maxhgt = param_find("SENS_FLOW_MAXHGT");
	_handle_sens_flow_maxr = param_find("SENS_FLOW_MAXR");
	_handle_sens_flow_minhgt = param_find("SENS_FLOW_MINHGT");
	_handle_sens_flow_rot = param_find("SENS_FLOW_ROT");
}