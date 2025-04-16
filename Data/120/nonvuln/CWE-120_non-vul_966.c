void enable_iommu(void)
{
	do_action_for_iommus(enable_dmar);
}