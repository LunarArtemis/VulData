static ssize_t
print_option(char *s, ssize_t len, int type, int dl, const uint8_t *data)
{
	const uint8_t *e, *t;
	uint16_t u16;
	int16_t s16;
	uint32_t u32;
	int32_t s32;
	struct in_addr addr;
	ssize_t bytes = 0;
	ssize_t l;
	char *tmp;

	if (type & RFC3397) {
		l = decode_rfc3397(NULL, 0, dl, data);
		if (l < 1)
			return l;
		tmp = xmalloc(l);
		decode_rfc3397(tmp, l, dl, data);
		l = print_string(s, len, l - 1, (uint8_t *)tmp);
		free(tmp);
		return l;
	}

	if (type & RFC3361) {
		if ((tmp = decode_rfc3361(dl, data)) == NULL)
			return -1;
		l = strlen(tmp);
		l = print_string(s, len, l - 1, (uint8_t *)tmp);
		free(tmp);
		return l;
	}

	if (type & RFC3442)
		return decode_rfc3442(s, len, dl, data);

	if (type & RFC5969)
		return decode_rfc5969(s, len, dl, data);

	if (type & STRING) {
		/* Some DHCP servers return NULL strings */
		if (*data == '\0')
			return 0;
		return print_string(s, len, dl, data);
	}

	if (!s) {
		if (type & UINT8)
			l = 3;
		else if (type & UINT16) {
			l = 5;
			dl /= 2;
		} else if (type & SINT16) {
			l = 6;
			dl /= 2;
		} else if (type & UINT32) {
			l = 10;
			dl /= 4;
		} else if (type & SINT32) {
			l = 11;
			dl /= 4;
		} else if (type & IPV4) {
			l = 16;
			dl /= 4;
		} else {
			errno = EINVAL;
			return -1;
		}
		return (l + 1) * dl;
	}

	t = data;
	e = data + dl;
	while (data < e) {
		if (data != t) {
			*s++ = ' ';
			bytes++;
			len--;
		}
		if (type & UINT8) {
			l = snprintf(s, len, "%d", *data);
			data++;
		} else if (type & UINT16) {
			memcpy(&u16, data, sizeof(u16));
			u16 = ntohs(u16);
			l = snprintf(s, len, "%d", u16);
			data += sizeof(u16);
		} else if (type & SINT16) {
			memcpy(&s16, data, sizeof(s16));
			s16 = ntohs(s16);
			l = snprintf(s, len, "%d", s16);
			data += sizeof(s16);
		} else if (type & UINT32) {
			memcpy(&u32, data, sizeof(u32));
			u32 = ntohl(u32);
			l = snprintf(s, len, "%d", u32);
			data += sizeof(u32);
		} else if (type & SINT32) {
			memcpy(&s32, data, sizeof(s32));
			s32 = ntohl(s32);
			l = snprintf(s, len, "%d", s32);
			data += sizeof(s32);
		} else if (type & IPV4) {
			memcpy(&addr.s_addr, data, sizeof(addr.s_addr));
			l = snprintf(s, len, "%s", inet_ntoa(addr));
			data += sizeof(addr.s_addr);
		} else
			l = 0;
		len -= l;
		bytes += l;
		s += l;
	}

	return bytes;
}