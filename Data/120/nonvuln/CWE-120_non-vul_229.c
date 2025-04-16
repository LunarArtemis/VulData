static irqreturn_t ismt_do_msi_interrupt(int vec, void *data)
{
	return ismt_handle_isr(data);
}