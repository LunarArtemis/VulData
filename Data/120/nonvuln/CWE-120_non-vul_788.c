bool MavlinkReceiver::component_was_seen(int system_id, int component_id)
{
	// For system broadcast messages return true if at least one component was seen before
	if (system_id == 0) {
		return _component_states_count > 0;
	}

	for (unsigned i = 0; i < _component_states_count; ++i) {
		if (_component_states[i].system_id == system_id
		    && (component_id == 0 || _component_states[i].component_id == component_id)) {
			return true;
		}
	}

	return false;
}