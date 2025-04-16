static inline u64
nvmet_fc_getassociationid(u64 connectionid)
{
	return connectionid & ~NVMET_FC_QUEUEID_MASK;
}