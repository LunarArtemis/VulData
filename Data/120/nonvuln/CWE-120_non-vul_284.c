static void block_sigs(sigset_t *oldset)
{
	sigset_t mask;

	siginitsetinv(&mask, sigmask(SIGKILL));
	sigprocmask(SIG_BLOCK, &mask, oldset);
}