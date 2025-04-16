static void
__ctnetlink_change_status(struct nf_conn *ct, unsigned long on,
			  unsigned long off)
{
	unsigned int bit;

	/* Ignore these unchangable bits */
	on &= ~IPS_UNCHANGEABLE_MASK;
	off &= ~IPS_UNCHANGEABLE_MASK;

	for (bit = 0; bit < __IPS_MAX_BIT; bit++) {
		if (on & (1 << bit))
			set_bit(bit, &ct->status);
		else if (off & (1 << bit))
			clear_bit(bit, &ct->status);
	}
}