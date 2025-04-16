static bool ctnetlink_needs_filter(u8 family, const struct nlattr * const *cda)
{
	return family || cda[CTA_MARK] || cda[CTA_FILTER];
}