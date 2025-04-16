static struct dmar_info *get_dmar_info(void)
{
#ifdef CONFIG_ACPI_PARSE_ENABLED
	parse_dmar_table(&plat_dmar_info);
#endif
	return &plat_dmar_info;
}