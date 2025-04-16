static DWORD WINAPI scanning_thread(LPVOID param)
#else
static void* scanning_thread(void* param)
#endif
{
  int result = ERROR_SUCCESS;
  THREAD_ARGS* args = (THREAD_ARGS*) param;

  char_t* file_path = file_queue_get(args->deadline);

  while (file_path != NULL)
  {
    args->callback_args.current_count = 0;
    args->callback_args.file_path = file_path;

    time_t current_time = time(NULL);

    if (current_time < args->deadline)
    {
      yr_scanner_set_timeout(
          args->scanner, (int) (args->deadline - current_time));

      result = scan_file(args->scanner, file_path);

      if (print_count_only)
      {
        cli_mutex_lock(&output_mutex);
        _tprintf(_T("%s: %d\n"), file_path, args->callback_args.current_count);
        cli_mutex_unlock(&output_mutex);
      }

      if (result != ERROR_SUCCESS)
      {
        cli_mutex_lock(&output_mutex);
        _ftprintf(stderr, _T("error scanning %s: "), file_path);
        print_scanner_error(args->scanner, result);
        cli_mutex_unlock(&output_mutex);
      }

      free(file_path);
      file_path = file_queue_get(args->deadline);
    }
    else
    {
      file_path = NULL;
    }
  }

  return 0;
}