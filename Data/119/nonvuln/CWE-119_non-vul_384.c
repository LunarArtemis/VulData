static ssize_t cqspi_write(struct spi_nor *nor, loff_t to,
			   size_t len, const u_char *buf)
{
	int ret;

	ret = cqspi_set_protocol(nor, 0);
	if (ret)
		return ret;

	ret = cqspi_indirect_write_setup(nor, to);
	if (ret)
		return ret;

	ret = cqspi_indirect_write_execute(nor, buf, len);
	if (ret)
		return ret;

	return (ret < 0) ? ret : len;
}