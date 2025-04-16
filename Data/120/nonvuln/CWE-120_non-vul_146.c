YR_API int yr_set_configuration(YR_CONFIG_NAME name, void *src)
{
  if (src == NULL)
    return ERROR_INTERNAL_FATAL_ERROR;

  switch (name)
  {  // lump all the cases using same types together in one cascade
  case YR_CONFIG_STACK_SIZE:
  case YR_CONFIG_MAX_STRINGS_PER_RULE:
  case YR_CONFIG_MAX_MATCH_DATA:
    yr_cfgs[name].ui32 = *(uint32_t *) src;
    break;

  case YR_CONFIG_MAX_PROCESS_MEMORY_CHUNK:
    yr_cfgs[name].ui64 = *(uint64_t *) src;
    break;

  default:
    return ERROR_INTERNAL_FATAL_ERROR;
  }

  return ERROR_SUCCESS;
}