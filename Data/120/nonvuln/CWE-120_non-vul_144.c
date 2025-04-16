YR_API int yr_initialize(void)
{
  YR_DEBUG_FPRINTF(2, stderr, "+ %s() {\n", __FUNCTION__);

  uint32_t def_stack_size = DEFAULT_STACK_SIZE;
  uint32_t def_max_strings_per_rule = DEFAULT_MAX_STRINGS_PER_RULE;
  uint32_t def_max_match_data = DEFAULT_MAX_MATCH_DATA;
  uint64_t def_max_process_memory_chunk = DEFAULT_MAX_PROCESS_MEMORY_CHUNK;

  init_count++;

  if (init_count > 1)
    return ERROR_SUCCESS;

  // Initialize random number generator, as it is used for generating object
  // canaries.
  srand((unsigned) time(NULL));

  for (int i = 0; i < 256; i++)
  {
    if (i >= 'a' && i <= 'z')
      yr_altercase[i] = i - 32;
    else if (i >= 'A' && i <= 'Z')
      yr_altercase[i] = i + 32;
    else
      yr_altercase[i] = i;

    yr_lowercase[i] = tolower(i);
  }

  FAIL_ON_ERROR(yr_heap_alloc());
  FAIL_ON_ERROR(yr_thread_storage_create(&yr_yyfatal_trampoline_tls));
  FAIL_ON_ERROR(yr_thread_storage_create(&yr_trycatch_trampoline_tls));

#if defined HAVE_LIBCRYPTO && OPENSSL_VERSION_NUMBER < 0x10100000L

  openssl_locks = (YR_MUTEX *) OPENSSL_malloc(
      CRYPTO_num_locks() * sizeof(YR_MUTEX));

  for (int i = 0; i < CRYPTO_num_locks(); i++)
    yr_mutex_create(&openssl_locks[i]);

  CRYPTO_THREADID_set_callback(_thread_id);
  CRYPTO_set_locking_callback(_locking_function);

#elif defined(HAVE_WINCRYPT_H)

  if (!CryptAcquireContext(
          &yr_cryptprov, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
  {
    return ERROR_INTERNAL_FATAL_ERROR;
  }

#elif defined(HAVE_COMMON_CRYPTO)

  ...

#endif

  FAIL_ON_ERROR(yr_modules_initialize());

  // Initialize default configuration options

  FAIL_ON_ERROR(yr_set_configuration(YR_CONFIG_STACK_SIZE, &def_stack_size));

  FAIL_ON_ERROR(yr_set_configuration(
      YR_CONFIG_MAX_STRINGS_PER_RULE, &def_max_strings_per_rule));

  FAIL_ON_ERROR(yr_set_configuration(
      YR_CONFIG_MAX_PROCESS_MEMORY_CHUNK, &def_max_process_memory_chunk));

  FAIL_ON_ERROR(
      yr_set_configuration(YR_CONFIG_MAX_MATCH_DATA, &def_max_match_data));

  YR_DEBUG_FPRINTF(2, stderr, "} // %s()\n", __FUNCTION__);

  return ERROR_SUCCESS;
}