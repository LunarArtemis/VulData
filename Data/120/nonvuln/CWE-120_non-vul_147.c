YR_API int yr_finalize(void)
{
  YR_DEBUG_FPRINTF(2, stderr, "+ %s() {\n", __FUNCTION__);

#if defined HAVE_LIBCRYPTO && OPENSSL_VERSION_NUMBER < 0x10100000L
  int i;
#endif

  // yr_finalize shouldn't be called without calling yr_initialize first

  if (init_count == 0)
    return ERROR_INTERNAL_FATAL_ERROR;

  init_count--;

  if (init_count > 0)
    return ERROR_SUCCESS;

#if defined HAVE_LIBCRYPTO && OPENSSL_VERSION_NUMBER < 0x10100000L

  for (i = 0; i < CRYPTO_num_locks(); i++) yr_mutex_destroy(&openssl_locks[i]);

  OPENSSL_free(openssl_locks);
  CRYPTO_THREADID_set_callback(NULL);
  CRYPTO_set_locking_callback(NULL);

#elif defined(HAVE_WINCRYPT_H)

  CryptReleaseContext(yr_cryptprov, 0);

#endif

  FAIL_ON_ERROR(yr_thread_storage_destroy(&yr_yyfatal_trampoline_tls));
  FAIL_ON_ERROR(yr_thread_storage_destroy(&yr_trycatch_trampoline_tls));
  FAIL_ON_ERROR(yr_modules_finalize());
  FAIL_ON_ERROR(yr_heap_free());

#if defined(JEMALLOC)
  malloc_stats_print(NULL, NULL, NULL);
  mallctl("prof.dump", NULL, NULL, NULL, 0);
#endif

  YR_DEBUG_FPRINTF(2, stderr, "} // %s()\n", __FUNCTION__);

  return ERROR_SUCCESS;
}