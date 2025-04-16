static int cqspi_wait_for_bit(void __iomem *reg, const u32 mask, bool clear)
{
	unsigned long end = jiffies + msecs_to_jiffies(CQSPI_TIMEOUT_MS);
	u32 val;

	while (1) {
		val = readl(reg);
		if (clear)
			val = ~val;
		val &= mask;

		if (val == mask)
			return 0;

		if (time_after(jiffies, end))
			return -ETIMEDOUT;
	}
}