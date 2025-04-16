static inline u16
nvmet_fc_getqueueid(u64 connectionid)
{
	return (u16)(connectionid & NVMET_FC_QUEUEID_MASK);
}