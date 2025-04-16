static int ismt_process_desc(const struct ismt_desc *desc,
			     union i2c_smbus_data *data,
			     struct ismt_priv *priv, int size,
			     char read_write)
{
	u8 *dma_buffer = PTR_ALIGN(&priv->buffer[0], 16);

	dev_dbg(&priv->pci_dev->dev, "Processing completed descriptor\n");
	__ismt_desc_dump(&priv->pci_dev->dev, desc);
	ismt_gen_reg_dump(priv);
	ismt_mstr_reg_dump(priv);

	if (desc->status & ISMT_DESC_SCS) {
		if (read_write == I2C_SMBUS_WRITE &&
		    size != I2C_SMBUS_PROC_CALL &&
		    size != I2C_SMBUS_BLOCK_PROC_CALL)
			return 0;

		switch (size) {
		case I2C_SMBUS_BYTE:
		case I2C_SMBUS_BYTE_DATA:
			data->byte = dma_buffer[0];
			break;
		case I2C_SMBUS_WORD_DATA:
		case I2C_SMBUS_PROC_CALL:
			data->word = dma_buffer[0] | (dma_buffer[1] << 8);
			break;
		case I2C_SMBUS_BLOCK_DATA:
		case I2C_SMBUS_BLOCK_PROC_CALL:
			if (desc->rxbytes != dma_buffer[0] + 1)
				return -EMSGSIZE;

			memcpy(data->block, dma_buffer, desc->rxbytes);
			break;
		case I2C_SMBUS_I2C_BLOCK_DATA:
			memcpy(&data->block[1], dma_buffer, desc->rxbytes);
			data->block[0] = desc->rxbytes;
			break;
		}
		return 0;
	}

	if (likely(desc->status & ISMT_DESC_NAK))
		return -ENXIO;

	if (desc->status & ISMT_DESC_CRC)
		return -EBADMSG;

	if (desc->status & ISMT_DESC_COL)
		return -EAGAIN;

	if (desc->status & ISMT_DESC_LPR)
		return -EPROTO;

	if (desc->status & (ISMT_DESC_DLTO | ISMT_DESC_CLTO))
		return -ETIMEDOUT;

	return -EIO;
}