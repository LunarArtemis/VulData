static int
nvme_fc_parse_traddr(struct nvmet_fc_traddr *traddr, char *buf, size_t blen)
{
	char name[2 + NVME_FC_TRADDR_HEXNAMELEN + 1];
	substring_t wwn = { name, &name[sizeof(name)-1] };
	int nnoffset, pnoffset;

	/* validate it string one of the 2 allowed formats */
	if (strnlen(buf, blen) == NVME_FC_TRADDR_MAXLENGTH &&
			!strncmp(buf, "nn-0x", NVME_FC_TRADDR_OXNNLEN) &&
			!strncmp(&buf[NVME_FC_TRADDR_MAX_PN_OFFSET],
				"pn-0x", NVME_FC_TRADDR_OXNNLEN)) {
		nnoffset = NVME_FC_TRADDR_OXNNLEN;
		pnoffset = NVME_FC_TRADDR_MAX_PN_OFFSET +
						NVME_FC_TRADDR_OXNNLEN;
	} else if ((strnlen(buf, blen) == NVME_FC_TRADDR_MINLENGTH &&
			!strncmp(buf, "nn-", NVME_FC_TRADDR_NNLEN) &&
			!strncmp(&buf[NVME_FC_TRADDR_MIN_PN_OFFSET],
				"pn-", NVME_FC_TRADDR_NNLEN))) {
		nnoffset = NVME_FC_TRADDR_NNLEN;
		pnoffset = NVME_FC_TRADDR_MIN_PN_OFFSET + NVME_FC_TRADDR_NNLEN;
	} else
		goto out_einval;

	name[0] = '0';
	name[1] = 'x';
	name[2 + NVME_FC_TRADDR_HEXNAMELEN] = 0;

	memcpy(&name[2], &buf[nnoffset], NVME_FC_TRADDR_HEXNAMELEN);
	if (__nvme_fc_parse_u64(&wwn, &traddr->nn))
		goto out_einval;

	memcpy(&name[2], &buf[pnoffset], NVME_FC_TRADDR_HEXNAMELEN);
	if (__nvme_fc_parse_u64(&wwn, &traddr->pn))
		goto out_einval;

	return 0;

out_einval:
	pr_warn("%s: bad traddr string\n", __func__);
	return -EINVAL;
}