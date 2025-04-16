YR_API void yr_scanner_set_callback(
    YR_SCANNER* scanner,
    YR_CALLBACK_FUNC callback,
    void* user_data)
{
  scanner->callback = callback;
  scanner->user_data = user_data;
}