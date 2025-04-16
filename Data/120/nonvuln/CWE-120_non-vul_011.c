static void
print_packet(u_char *user, const struct pcap_pkthdr *h, const u_char *sp)
{
	++packets_captured;

	++infodelay;

	pretty_print_packet((netdissect_options *)user, h, sp, packets_captured);

	--infodelay;
	if (infoprint)
		info(0);
}