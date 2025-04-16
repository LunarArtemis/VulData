YR_API int yr_scanner_print_profiling_info(YR_SCANNER* scanner)
{
  printf("\n===== PROFILING INFORMATION =====\n\n");

  YR_RULE_PROFILING_INFO* info = yr_scanner_get_profiling_info(scanner);

  if (info == NULL)
    return ERROR_INSUFFICIENT_MEMORY;

  YR_RULE_PROFILING_INFO* rpi = info;

  while (rpi->rule != NULL)
  {
    printf(
        "%10" PRIu64 " %s:%s: \n",
        rpi->cost,
        rpi->rule->ns->name,
        rpi->rule->identifier);

    rpi++;
  }

  printf("\n=================================\n");

  yr_free(info);

  return ERROR_SUCCESS;
}