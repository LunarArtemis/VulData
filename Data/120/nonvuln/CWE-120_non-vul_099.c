YR_API void yr_compiler_set_atom_quality_table(
    YR_COMPILER* compiler,
    const void* table,
    int entries,
    unsigned char warning_threshold)
{
  compiler->atoms_config.free_quality_table = false;
  compiler->atoms_config.quality_warning_threshold = warning_threshold;
  compiler->atoms_config.get_atom_quality = yr_atoms_table_quality;
  compiler->atoms_config.quality_table_entries = entries;
  compiler->atoms_config.quality_table = (YR_ATOM_QUALITY_TABLE_ENTRY*) table;
}