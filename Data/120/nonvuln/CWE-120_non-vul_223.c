static void __ismt_desc_dump(struct device *dev, const struct ismt_desc *desc)
{

	dev_dbg(dev, "Descriptor struct:  %p\n", desc);
	dev_dbg(dev, "\ttgtaddr_rw=0x%02X\n", desc->tgtaddr_rw);
	dev_dbg(dev, "\twr_len_cmd=0x%02X\n", desc->wr_len_cmd);
	dev_dbg(dev, "\trd_len=    0x%02X\n", desc->rd_len);
	dev_dbg(dev, "\tcontrol=   0x%02X\n", desc->control);
	dev_dbg(dev, "\tstatus=    0x%02X\n", desc->status);
	dev_dbg(dev, "\tretry=     0x%02X\n", desc->retry);
	dev_dbg(dev, "\trxbytes=   0x%02X\n", desc->rxbytes);
	dev_dbg(dev, "\ttxbytes=   0x%02X\n", desc->txbytes);
	dev_dbg(dev, "\tdptr_low=  0x%08X\n", desc->dptr_low);
	dev_dbg(dev, "\tdptr_high= 0x%08X\n", desc->dptr_high);
}