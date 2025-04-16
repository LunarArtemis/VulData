void destroy_iommu_domain(struct iommu_domain *domain)
{
	/* TODO: check if any device assigned to this domain */
	(void)memset(domain, 0U, sizeof(*domain));
}