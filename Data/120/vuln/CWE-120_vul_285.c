struct Address *
new_address(const char *hostname_or_ip) {
    union {
        struct sockaddr a;
        struct sockaddr_in in;
        struct sockaddr_in6 in6;
        struct sockaddr_un un;
        struct sockaddr_storage s;
    } s;
    char ip_buf[ADDRESS_BUFFER_SIZE];
    char *port;
    size_t len;

    if (hostname_or_ip == NULL)
        return NULL;

    /* IPv6 address */
    /* we need to test for raw IPv6 address for IPv4 port combinations since a
     * colon would give false positives
     */
    memset(&s, 0, sizeof(s));
    if (inet_pton(AF_INET6, hostname_or_ip,
                &s.in6.sin6_addr) == 1) {
        s.in6.sin6_family = AF_INET6;

        return new_address_sa(&s.a, sizeof(s.in6));
    }

    /* Unix socket */
    memset(&s, 0, sizeof(s));
    if (strncmp("unix:", hostname_or_ip, 5) == 0) {
        if (strlen(hostname_or_ip) >=
                sizeof(s.un.sun_path))
            return NULL;

        /* XXX: only supporting pathname unix sockets */
        s.un.sun_family = AF_UNIX;
        strncpy(s.un.sun_path,
                hostname_or_ip + 5,
                sizeof(s.un.sun_path) - 1);

        return new_address_sa(&s.a, offsetof(struct sockaddr_un, sun_path) +
                              strlen(s.un.sun_path) + 1);
    }

    /* Trailing port */
    if ((port = strrchr(hostname_or_ip, ':')) != NULL &&
            is_numeric(port + 1)) {
        len = (size_t)(port - hostname_or_ip);
        int port_num = atoi(port + 1);

        if (len < sizeof(ip_buf) && port_num >= 0 && port_num <= 65535) {
            strncpy(ip_buf, hostname_or_ip, len);
            ip_buf[len] = '\0';

            struct Address *addr = new_address(ip_buf);
            if (addr != NULL)
                address_set_port(addr, (uint16_t) port_num);

            return addr;
        }
    }

    /* Wildcard */
    if (strcmp("*", hostname_or_ip) == 0) {
        struct Address *addr = malloc(sizeof(struct Address));
        if (addr != NULL) {
            addr->type = WILDCARD;
            addr->len = 0;
            address_set_port(addr, 0);
        }
        return addr;
    }

    /* IPv4 address */
    memset(&s, 0, sizeof(s));
    if (inet_pton(AF_INET, hostname_or_ip,
                  &s.in.sin_addr) == 1) {
        s.in.sin_family = AF_INET;

        return new_address_sa(&s.a, sizeof(s.in));
    }

    /* [IPv6 address] */
    memset(&s, 0, sizeof(s));
    if (hostname_or_ip[0] == '[' &&
            (port = strchr(hostname_or_ip, ']')) != NULL) {
        len = (size_t)(port - hostname_or_ip - 1);

        /* inet_pton() will not parse the IP correctly unless it is in a
         * separate string.
         */
        strncpy(ip_buf, hostname_or_ip + 1, len);
        ip_buf[len] = '\0';

        if (inet_pton(AF_INET6, ip_buf,
                      &s.in6.sin6_addr) == 1) {
            s.in6.sin6_family = AF_INET6;

            return new_address_sa(&s.a, sizeof(s.in6));
        }
    }

    /* hostname */
    if (valid_hostname(hostname_or_ip)) {
        len = strlen(hostname_or_ip);
        struct Address *addr = malloc(
                offsetof(struct Address, data) + len + 1);
        if (addr != NULL) {
            addr->type = HOSTNAME;
            addr->port = 0;
            addr->len = len;
            memcpy(addr->data, hostname_or_ip, len);
            addr->data[addr->len] = '\0';

            /* Store address in lower case */
            for (char *c = addr->data; *c != '\0'; c++)
                *c = tolower(*c);
        }

        return addr;
    }

    return NULL;
}