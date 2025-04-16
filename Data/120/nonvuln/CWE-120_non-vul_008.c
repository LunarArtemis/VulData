static void
compress_savefile(const char *filename)
{
	pid_t child;

	child = fork_subprocess();
	if (child == -1) {
		fprintf(stderr,
			"compress_savefile: fork failed: %s\n",
			pcap_strerror(errno));
		return;
	}
	if (child != 0) {
		/* Parent process. */
		return;
	}

	/*
	 * Child process.
	 * Set to lowest priority so that this doesn't disturb the capture.
	 */
#ifdef NZERO
	setpriority(PRIO_PROCESS, 0, NZERO - 1);
#else
	setpriority(PRIO_PROCESS, 0, 19);
#endif
	if (execlp(zflag, zflag, filename, (char *)NULL) == -1)
		fprintf(stderr,
			"compress_savefile: execlp(%s, %s) failed: %s\n",
			zflag,
			filename,
			pcap_strerror(errno));
#ifdef HAVE_FORK
	exit(1);
#else
	_exit(1);
#endif
}