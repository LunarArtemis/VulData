static bool
queue_90percent_full(struct nvmet_fc_tgt_queue *q, u32 sqhd)
{
	u32 sqtail, used;

	/* egad, this is ugly. And sqtail is just a best guess */
	sqtail = atomic_read(&q->sqtail) % q->sqsize;

	used = (sqtail < sqhd) ? (sqtail + q->sqsize - sqhd) : (sqtail - sqhd);
	return ((used * 10) >= (((u32)(q->sqsize - 1) * 9)));
}