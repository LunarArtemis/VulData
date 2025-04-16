static char *
find_interface_by_number(long devnum)
{
	pcap_if_t *dev, *devlist;
	long i;
	char ebuf[PCAP_ERRBUF_SIZE];
	char *device;

	if (pcap_findalldevs(&devlist, ebuf) < 0)
		error("%s", ebuf);
	/*
	 * Look for the devnum-th entry in the list of devices (1-based).
	 */
	for (i = 0, dev = devlist; i < devnum-1 && dev != NULL;
	    i++, dev = dev->next)
		;
	if (dev == NULL)
		error("Invalid adapter index");
	device = strdup(dev->name);
	pcap_freealldevs(devlist);
	return (device);
}