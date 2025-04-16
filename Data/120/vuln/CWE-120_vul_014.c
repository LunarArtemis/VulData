YR_API int yr_get_configuration(YR_CONFIG_NAME name, void *dest)
{
  if (dest == NULL)
    return ERROR_INTERNAL_FATAL_ERROR;

  switch (name)
  {  // lump all the cases using same types together in one cascade
  case YR_CONFIG_STACK_SIZE:
  case YR_CONFIG_MAX_STRINGS_PER_RULE:
  case YR_CONFIG_MAX_MATCH_DATA:
    *(uint32_t *) dest = yr_cfgs[name].ui32;
    break;

  case YR_CONFIG_MAX_PROCESS_MEMORY_CHUNK:
    *(uint64_t *) dest = yr_cfgs[name].ui64;
    break;

  default:
    return ERROR_INTERNAL_FATAL_ERROR;
  }

  return ERROR_SUCCESS;
}