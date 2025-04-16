static int cqspi_write_reg(struct spi_nor *nor, u8 opcode, u8 *buf, int len)
{
	int ret;

	ret = cqspi_set_protocol(nor, 0);
	if (!ret)
		ret = cqspi_command_write(nor, opcode, buf, len);

	return ret;
}