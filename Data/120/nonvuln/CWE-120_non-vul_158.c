YR_API void yr_scanner_reset_profiling_info(YR_SCANNER* scanner)
{
#ifdef YR_PROFILING_ENABLED
  memset(
      scanner->profiling_info,
      0,
      scanner->rules->num_rules * sizeof(YR_PROFILING_INFO));
#endif
}