void
MavlinkReceiver::handle_message_serial_control(mavlink_message_t *msg)
{
	mavlink_serial_control_t serial_control_mavlink;
	mavlink_msg_serial_control_decode(msg, &serial_control_mavlink);

	// we only support shell commands
	if (serial_control_mavlink.device != SERIAL_CONTROL_DEV_SHELL
	    || (serial_control_mavlink.flags & SERIAL_CONTROL_FLAG_REPLY)) {
		return;
	}

	MavlinkShell *shell = _mavlink->get_shell();

	if (shell) {
		// we ignore the timeout, EXCLUSIVE & BLOCKING flags of the SERIAL_CONTROL message
		if (serial_control_mavlink.count > 0) {
			shell->write(serial_control_mavlink.data, serial_control_mavlink.count);
		}

		// if no response requested, assume the shell is no longer used
		if ((serial_control_mavlink.flags & SERIAL_CONTROL_FLAG_RESPOND) == 0) {
			_mavlink->close_shell();
		}
	}
}