static int tcm_loop_slave_alloc(struct scsi_device *sd)
{
	set_bit(QUEUE_FLAG_BIDI, &sd->request_queue->queue_flags);
	return 0;
}