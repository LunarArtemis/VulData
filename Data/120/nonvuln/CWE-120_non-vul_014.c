USES_APPLE_DEPRECATED_API
static void
print_version(void)
{
	extern char version[];
#ifndef HAVE_PCAP_LIB_VERSION
#if defined(_WIN32) || defined(HAVE_PCAP_VERSION)
	extern char pcap_version[];
#else /* defined(_WIN32) || defined(HAVE_PCAP_VERSION) */
	static char pcap_version[] = "unknown";
#endif /* defined(_WIN32) || defined(HAVE_PCAP_VERSION) */
#endif /* HAVE_PCAP_LIB_VERSION */
	const char *smi_version_string;

#ifdef HAVE_PCAP_LIB_VERSION
#ifdef _WIN32
	(void)fprintf(stderr, "%s version %s, based on tcpdump version %s\n", program_name, WDversion, version);
#else /* _WIN32 */
	(void)fprintf(stderr, "%s version %s\n", program_name, version);
#endif /* _WIN32 */
	(void)fprintf(stderr, "%s\n",pcap_lib_version());
#else /* HAVE_PCAP_LIB_VERSION */
#ifdef _WIN32
	(void)fprintf(stderr, "%s version %s, based on tcpdump version %s\n", program_name, WDversion, version);
	(void)fprintf(stderr, "WinPcap version %s, based on libpcap version %s\n",Wpcap_version, pcap_version);
#else /* _WIN32 */
	(void)fprintf(stderr, "%s version %s\n", program_name, version);
	(void)fprintf(stderr, "libpcap version %s\n", pcap_version);
#endif /* _WIN32 */
#endif /* HAVE_PCAP_LIB_VERSION */

#if defined(HAVE_LIBCRYPTO) && defined(SSLEAY_VERSION)
	(void)fprintf (stderr, "%s\n", SSLeay_version(SSLEAY_VERSION));
#endif

	smi_version_string = nd_smi_version_string();
	if (smi_version_string != NULL)
		(void)fprintf (stderr, "SMI-library: %s\n", smi_version_string);

#if defined(__SANITIZE_ADDRESS__)
	(void)fprintf (stderr, "Compiled with AddressSanitizer/GCC.\n");
#elif defined(__has_feature)
#  if __has_feature(address_sanitizer)
	(void)fprintf (stderr, "Compiled with AddressSanitizer/CLang.\n");
#  endif
#endif /* __SANITIZE_ADDRESS__ or __has_feature */
}