void
MavlinkReceiver::updateParams()
{
	// update parameters from storage
	ModuleParams::updateParams();

	if (_handle_sens_flow_maxhgt != PARAM_INVALID) {
		param_get(_handle_sens_flow_maxhgt, &_param_sens_flow_maxhgt);
	}

	if (_handle_sens_flow_maxr != PARAM_INVALID) {
		param_get(_handle_sens_flow_maxr, &_param_sens_flow_maxr);
	}

	if (_handle_sens_flow_minhgt != PARAM_INVALID) {
		param_get(_handle_sens_flow_minhgt, &_param_sens_flow_minhgt);
	}

	if (_handle_sens_flow_rot != PARAM_INVALID) {
		param_get(_handle_sens_flow_rot, &_param_sens_flow_rot);
	}
}