void pid_ns_release_proc(struct pid_namespace *ns)
{
	mntput(ns->proc_mnt);
}