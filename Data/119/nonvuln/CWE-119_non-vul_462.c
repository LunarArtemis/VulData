static inline int
nvmet_fc_fodnum(struct nvmet_fc_fcp_iod *fodptr)
{
	return (fodptr - fodptr->queue->fod);
}