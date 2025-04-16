static void
show_devices_and_exit (void)
{
	pcap_if_t *dev, *devlist;
	char ebuf[PCAP_ERRBUF_SIZE];
	int i;

	if (pcap_findalldevs(&devlist, ebuf) < 0)
		error("%s", ebuf);
	for (i = 0, dev = devlist; dev != NULL; i++, dev = dev->next) {
		printf("%d.%s", i+1, dev->name);
		if (dev->description != NULL)
			printf(" (%s)", dev->description);
		if (dev->flags != 0)
			printf(" [%s]", bittok2str(status_flags, "none", dev->flags));
		printf("\n");
	}
	pcap_freealldevs(devlist);
	exit_tcpdump(0);
}