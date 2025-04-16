static inline struct nvmet_fc_tgtport *
targetport_to_tgtport(struct nvmet_fc_target_port *targetport)
{
	return container_of(targetport, struct nvmet_fc_tgtport,
				 fc_target_port);
}