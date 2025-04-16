int zsock_getaddrinfo(const char *host, const char *service,
		      const struct zsock_addrinfo *hints,
		      struct zsock_addrinfo **res)
{
	int family = AF_UNSPEC;
	long int port;
	int st1 = DNS_EAI_ADDRFAMILY, st2 = DNS_EAI_ADDRFAMILY;

	if (hints) {
		family = hints->ai_family;
	}

	port = strtol(service, NULL, 10);
	if (port < 1 || port > 65535) {
		return DNS_EAI_NONAME;
	}

	ai_state.hints = hints;
	ai_state.idx = 0;
	k_sem_init(&ai_state.sem, 0, UINT_MAX);

	/* Link entries in advance */
	ai_arr[0].ai_next = &ai_arr[1];

	/* Execute if AF_UNSPEC or AF_INET4 */
	if (family != AF_INET6) {
		dns_get_addr_info(host, DNS_QUERY_TYPE_A, NULL,
				  dns_resolve_cb, &ai_state, 1000);
		k_sem_take(&ai_state.sem, K_FOREVER);
		net_sin(&ai_arr[ai_state.idx - 1]._ai_addr)->sin_port =
								htons(port);
		st1 = ai_state.status;
	}

	/* Execute if AF_UNSPEC or AF_INET6 */
	if (family != AF_INET) {
		dns_get_addr_info(host, DNS_QUERY_TYPE_AAAA, NULL,
				  dns_resolve_cb, &ai_state, 1000);
		k_sem_take(&ai_state.sem, K_FOREVER);
		net_sin6(&ai_arr[ai_state.idx - 1]._ai_addr)->sin6_port =
								htons(port);
		st2 = ai_state.status;
	}

	/* If both attempts failed, it's error */
	if (st1 && st2) {
		if (st1 != DNS_EAI_ADDRFAMILY) {
			return st1;
		}
		return st2;
	}

	/* Mark entry as last */
	ai_arr[ai_state.idx - 1].ai_next = NULL;

	*res = ai_arr;

	return 0;
}