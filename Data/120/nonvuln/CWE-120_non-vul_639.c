static int
ctnetlink_update_status(struct nf_conn *ct, const struct nlattr * const cda[])
{
	unsigned int status = ntohl(nla_get_be32(cda[CTA_STATUS]));
	unsigned long d = ct->status ^ status;

	if (d & IPS_SEEN_REPLY && !(status & IPS_SEEN_REPLY))
		/* SEEN_REPLY bit can only be set */
		return -EBUSY;

	if (d & IPS_ASSURED && !(status & IPS_ASSURED))
		/* ASSURED bit can only be set */
		return -EBUSY;

	/* This check is less strict than ctnetlink_change_status()
	 * because callers often flip IPS_EXPECTED bits when sending
	 * an NFQA_CT attribute to the kernel.  So ignore the
	 * unchangeable bits but do not error out. Also user programs
	 * are allowed to clear the bits that they are allowed to change.
	 */
	__ctnetlink_change_status(ct, status, ~status);
	return 0;
}