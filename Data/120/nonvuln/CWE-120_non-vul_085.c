YR_API int yr_compiler_load_atom_quality_table(
    YR_COMPILER* compiler,
    const char* filename,
    unsigned char warning_threshold)
{
  long file_size;
  int entries;
  void* table;

  FILE* fh = fopen(filename, "rb");

  if (fh == NULL)
    return ERROR_COULD_NOT_OPEN_FILE;

  fseek(fh, 0L, SEEK_END);
  file_size = ftell(fh);
  fseek(fh, 0L, SEEK_SET);

  if (file_size == -1L)
  {
    fclose(fh);
    return ERROR_COULD_NOT_READ_FILE;
  }

  table = yr_malloc(file_size);

  if (table == NULL)
  {
    fclose(fh);
    return ERROR_INSUFFICIENT_MEMORY;
  }

  entries = (int) file_size / sizeof(YR_ATOM_QUALITY_TABLE_ENTRY);

  if (fread(table, sizeof(YR_ATOM_QUALITY_TABLE_ENTRY), entries, fh) != entries)
  {
    fclose(fh);
    yr_free(table);
    return ERROR_COULD_NOT_READ_FILE;
  }

  fclose(fh);

  yr_compiler_set_atom_quality_table(
      compiler, table, entries, warning_threshold);

  compiler->atoms_config.free_quality_table = true;

  return ERROR_SUCCESS;
}