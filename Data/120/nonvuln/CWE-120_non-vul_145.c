static void _locking_function(int mode, int n, const char *file, int line)
{
  if (mode & CRYPTO_LOCK)
    yr_mutex_lock(&openssl_locks[n]);
  else
    yr_mutex_unlock(&openssl_locks[n]);
}