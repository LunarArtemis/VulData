static irqreturn_t si4713_handler(int irq, void *dev)
{
	struct si4713_device *sdev = dev;

	v4l2_dbg(2, debug, &sdev->sd,
			"%s: sending signal to completion work.\n", __func__);
	complete(&sdev->work);

	return IRQ_HANDLED;
}