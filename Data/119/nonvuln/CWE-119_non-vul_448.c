static inline int
nvmet_fc_iodnum(struct nvmet_fc_ls_iod *iodptr)
{
	return (iodptr - iodptr->tgtport->iod);
}