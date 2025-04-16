static int cqspi_erase(struct spi_nor *nor, loff_t offs)
{
	int ret;

	ret = cqspi_set_protocol(nor, 0);
	if (ret)
		return ret;

	/* Send write enable, then erase commands. */
	ret = nor->write_reg(nor, SPINOR_OP_WREN, NULL, 0);
	if (ret)
		return ret;

	/* Set up command buffer. */
	ret = cqspi_command_write_addr(nor, nor->erase_opcode, offs);
	if (ret)
		return ret;

	return 0;
}