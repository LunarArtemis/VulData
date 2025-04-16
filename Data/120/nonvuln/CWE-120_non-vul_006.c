static void
show_tstamp_types_and_exit(pcap_t *pc, const char *device)
{
	int n_tstamp_types;
	int *tstamp_types = 0;
	const char *tstamp_type_name;
	int i;

	n_tstamp_types = pcap_list_tstamp_types(pc, &tstamp_types);
	if (n_tstamp_types < 0)
		error("%s", pcap_geterr(pc));

	if (n_tstamp_types == 0) {
		fprintf(stderr, "Time stamp type cannot be set for %s\n",
		    device);
		exit_tcpdump(0);
	}
	fprintf(stderr, "Time stamp types for %s (use option -j to set):\n",
	    device);
	for (i = 0; i < n_tstamp_types; i++) {
		tstamp_type_name = pcap_tstamp_type_val_to_name(tstamp_types[i]);
		if (tstamp_type_name != NULL) {
			(void) fprintf(stderr, "  %s (%s)\n", tstamp_type_name,
			    pcap_tstamp_type_val_to_description(tstamp_types[i]));
		} else {
			(void) fprintf(stderr, "  %d\n", tstamp_types[i]);
		}
	}
	pcap_free_tstamp_types(tstamp_types);
	exit_tcpdump(0);
}