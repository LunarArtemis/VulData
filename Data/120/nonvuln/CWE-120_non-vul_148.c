YR_API void yr_scanner_set_timeout(YR_SCANNER* scanner, int timeout)
{
  // Convert timeout from seconds to nanoseconds.
  scanner->timeout = timeout * 1000000000ULL;
}