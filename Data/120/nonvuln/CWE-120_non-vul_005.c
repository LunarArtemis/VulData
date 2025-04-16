USES_APPLE_RST

static void
print_usage(void)
{
	print_version();
	(void)fprintf(stderr,
"Usage: %s [-aAbd" D_FLAG "efhH" I_FLAG J_FLAG "KlLnNOpqStu" U_FLAG "vxX#]" B_FLAG_USAGE " [ -c count ]\n", program_name);
	(void)fprintf(stderr,
"\t\t[ -C file_size ] [ -E algo:secret ] [ -F file ] [ -G seconds ]\n");
	(void)fprintf(stderr,
"\t\t[ -i interface ]" j_FLAG_USAGE " [ -M secret ] [ --number ]\n");
#ifdef HAVE_PCAP_SETDIRECTION
	(void)fprintf(stderr,
"\t\t[ -Q in|out|inout ]\n");
#endif
	(void)fprintf(stderr,
"\t\t[ -r file ] [ -s snaplen ] ");
#ifdef HAVE_PCAP_SET_TSTAMP_PRECISION
	(void)fprintf(stderr, "[ --time-stamp-precision precision ]\n");
	(void)fprintf(stderr,
"\t\t");
#endif
#ifdef HAVE_PCAP_SET_IMMEDIATE_MODE
	(void)fprintf(stderr, "[ --immediate-mode ] ");
#endif
	(void)fprintf(stderr, "[ -T type ] [ --version ] [ -V file ]\n");
	(void)fprintf(stderr,
"\t\t[ -w file ] [ -W filecount ] [ -y datalinktype ] [ -z postrotate-command ]\n");
	(void)fprintf(stderr,
"\t\t[ -Z user ] [ expression ]\n");
}