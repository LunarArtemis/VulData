static ssize_t cqspi_read(struct spi_nor *nor, loff_t from,
			  size_t len, u_char *buf)
{
	int ret;

	ret = cqspi_set_protocol(nor, 1);
	if (ret)
		return ret;

	ret = cqspi_indirect_read_setup(nor, from);
	if (ret)
		return ret;

	ret = cqspi_indirect_read_execute(nor, buf, len);
	if (ret)
		return ret;

	return (ret < 0) ? ret : len;
}