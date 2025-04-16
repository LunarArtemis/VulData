static long
parse_interface_number(const char *device)
{
	long devnum;
	char *end;

	devnum = strtol(device, &end, 10);
	if (device != end && *end == '\0') {
		/*
		 * It's all-numeric, but is it a valid number?
		 */
		if (devnum <= 0) {
			/*
			 * No, it's not an ordinal.
			 */
			error("Invalid adapter index");
		}
		return (devnum);
	} else {
		/*
		 * It's not all-numeric; return -1, so our caller
		 * knows that.
		 */
		return (-1);
	}
}