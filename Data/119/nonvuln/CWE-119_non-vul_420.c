static inline struct nvmet_fc_fcp_iod *
nvmet_req_to_fod(struct nvmet_req *nvme_req)
{
	return container_of(nvme_req, struct nvmet_fc_fcp_iod, req);
}