void resume_iommu(void)
{
	do_action_for_iommus(resume_dmar);
}