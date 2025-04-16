static inline bool is_dmar_unit_ignored(const struct dmar_drhd_rt *dmar_unit)
{
	bool ignored = false;

	if ((dmar_unit != NULL) && (dmar_unit->drhd->ignore)) {
		ignored = true;
	}

	return ignored;
}