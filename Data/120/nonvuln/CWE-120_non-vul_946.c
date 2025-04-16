void suspend_iommu(void)
{
	do_action_for_iommus(suspend_dmar);
}