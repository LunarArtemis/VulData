static void
exit_tcpdump(int status)
{
	nd_cleanup();
	exit(status);
}