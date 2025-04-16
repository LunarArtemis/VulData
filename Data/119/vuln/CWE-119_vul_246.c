static int
do_ip_vs_get_ctl(struct sock *sk, int cmd, void __user *user, int *len)
{
	unsigned char arg[128];
	int ret = 0;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (*len < get_arglen[GET_CMDID(cmd)]) {
		pr_err("get_ctl: len %u < %u\n",
		       *len, get_arglen[GET_CMDID(cmd)]);
		return -EINVAL;
	}

	if (copy_from_user(arg, user, get_arglen[GET_CMDID(cmd)]) != 0)
		return -EFAULT;

	if (mutex_lock_interruptible(&__ip_vs_mutex))
		return -ERESTARTSYS;

	switch (cmd) {
	case IP_VS_SO_GET_VERSION:
	{
		char buf[64];

		sprintf(buf, "IP Virtual Server version %d.%d.%d (size=%d)",
			NVERSION(IP_VS_VERSION_CODE), IP_VS_CONN_TAB_SIZE);
		if (copy_to_user(user, buf, strlen(buf)+1) != 0) {
			ret = -EFAULT;
			goto out;
		}
		*len = strlen(buf)+1;
	}
	break;

	case IP_VS_SO_GET_INFO:
	{
		struct ip_vs_getinfo info;
		info.version = IP_VS_VERSION_CODE;
		info.size = IP_VS_CONN_TAB_SIZE;
		info.num_services = ip_vs_num_services;
		if (copy_to_user(user, &info, sizeof(info)) != 0)
			ret = -EFAULT;
	}
	break;

	case IP_VS_SO_GET_SERVICES:
	{
		struct ip_vs_get_services *get;
		int size;

		get = (struct ip_vs_get_services *)arg;
		size = sizeof(*get) +
			sizeof(struct ip_vs_service_entry) * get->num_services;
		if (*len != size) {
			pr_err("length: %u != %u\n", *len, size);
			ret = -EINVAL;
			goto out;
		}
		ret = __ip_vs_get_service_entries(get, user);
	}
	break;

	case IP_VS_SO_GET_SERVICE:
	{
		struct ip_vs_service_entry *entry;
		struct ip_vs_service *svc;
		union nf_inet_addr addr;

		entry = (struct ip_vs_service_entry *)arg;
		addr.ip = entry->addr;
		if (entry->fwmark)
			svc = __ip_vs_svc_fwm_get(AF_INET, entry->fwmark);
		else
			svc = __ip_vs_service_get(AF_INET, entry->protocol,
						  &addr, entry->port);
		if (svc) {
			ip_vs_copy_service(entry, svc);
			if (copy_to_user(user, entry, sizeof(*entry)) != 0)
				ret = -EFAULT;
			ip_vs_service_put(svc);
		} else
			ret = -ESRCH;
	}
	break;

	case IP_VS_SO_GET_DESTS:
	{
		struct ip_vs_get_dests *get;
		int size;

		get = (struct ip_vs_get_dests *)arg;
		size = sizeof(*get) +
			sizeof(struct ip_vs_dest_entry) * get->num_dests;
		if (*len != size) {
			pr_err("length: %u != %u\n", *len, size);
			ret = -EINVAL;
			goto out;
		}
		ret = __ip_vs_get_dest_entries(get, user);
	}
	break;

	case IP_VS_SO_GET_TIMEOUT:
	{
		struct ip_vs_timeout_user t;

		__ip_vs_get_timeouts(&t);
		if (copy_to_user(user, &t, sizeof(t)) != 0)
			ret = -EFAULT;
	}
	break;

	case IP_VS_SO_GET_DAEMON:
	{
		struct ip_vs_daemon_user d[2];

		memset(&d, 0, sizeof(d));
		if (ip_vs_sync_state & IP_VS_STATE_MASTER) {
			d[0].state = IP_VS_STATE_MASTER;
			strlcpy(d[0].mcast_ifn, ip_vs_master_mcast_ifn, sizeof(d[0].mcast_ifn));
			d[0].syncid = ip_vs_master_syncid;
		}
		if (ip_vs_sync_state & IP_VS_STATE_BACKUP) {
			d[1].state = IP_VS_STATE_BACKUP;
			strlcpy(d[1].mcast_ifn, ip_vs_backup_mcast_ifn, sizeof(d[1].mcast_ifn));
			d[1].syncid = ip_vs_backup_syncid;
		}
		if (copy_to_user(user, &d, sizeof(d)) != 0)
			ret = -EFAULT;
	}
	break;

	default:
		ret = -EINVAL;
	}

  out:
	mutex_unlock(&__ip_vs_mutex);
	return ret;
}