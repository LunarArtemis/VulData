static void
droproot(const char *username, const char *chroot_dir)
{
	struct passwd *pw = NULL;

	if (chroot_dir && !username) {
		fprintf(stderr, "%s: Chroot without dropping root is insecure\n",
			program_name);
		exit_tcpdump(1);
	}

	pw = getpwnam(username);
	if (pw) {
		if (chroot_dir) {
			if (chroot(chroot_dir) != 0 || chdir ("/") != 0) {
				fprintf(stderr, "%s: Couldn't chroot/chdir to '%.64s': %s\n",
					program_name, chroot_dir, pcap_strerror(errno));
				exit_tcpdump(1);
			}
		}
#ifdef HAVE_LIBCAP_NG
		{
			int ret = capng_change_id(pw->pw_uid, pw->pw_gid, CAPNG_NO_FLAG);
			if (ret < 0)
				error("capng_change_id(): return %d\n", ret);
			else
				fprintf(stderr, "dropped privs to %s\n", username);
		}
#else
		if (initgroups(pw->pw_name, pw->pw_gid) != 0 ||
		    setgid(pw->pw_gid) != 0 || setuid(pw->pw_uid) != 0) {
			fprintf(stderr, "%s: Couldn't change to '%.32s' uid=%lu gid=%lu: %s\n",
				program_name, username,
				(unsigned long)pw->pw_uid,
				(unsigned long)pw->pw_gid,
				pcap_strerror(errno));
			exit_tcpdump(1);
		}
		else {
			fprintf(stderr, "dropped privs to %s\n", username);
		}
#endif /* HAVE_LIBCAP_NG */
	}
	else {
		fprintf(stderr, "%s: Couldn't find user '%.32s'\n",
			program_name, username);
		exit_tcpdump(1);
	}
#ifdef HAVE_LIBCAP_NG
	/* We don't need CAP_SETUID, CAP_SETGID and CAP_SYS_CHROOT any more. */
	capng_updatev(
		CAPNG_DROP,
		CAPNG_EFFECTIVE | CAPNG_PERMITTED,
		CAP_SETUID,
		CAP_SETGID,
		CAP_SYS_CHROOT,
		-1);
	capng_apply(CAPNG_SELECT_BOTH);
#endif /* HAVE_LIBCAP_NG */

}