int _tmain(int argc, const char_t** argv)
{
  COMPILER_RESULTS cr;

  YR_COMPILER* compiler = NULL;
  YR_RULES* rules = NULL;
  YR_SCANNER* scanner = NULL;
  SCAN_OPTIONS scan_opts;

  bool arg_is_dir = false;
  int flags = 0;
  int result;

  argc = args_parse(options, argc, argv);

  scan_opts.follow_symlinks = follow_symlinks;
  scan_opts.recursive_search = recursive_search;

  if (show_version)
  {
    printf("%s\n", YR_VERSION);
    return EXIT_SUCCESS;
  }

  if (show_help)
  {
    printf(
        "YARA %s, the pattern matching swiss army knife.\n"
        "%s\n\n"
        "Mandatory arguments to long options are mandatory for "
        "short options too.\n\n",
        YR_VERSION,
        USAGE_STRING);

    args_print_usage(options, 43);
    printf(
        "\nSend bug reports and suggestions to: vmalvarez@virustotal.com.\n");

    return EXIT_SUCCESS;
  }

  if (threads > YR_MAX_THREADS)
  {
    fprintf(stderr, "maximum number of threads is %d\n", YR_MAX_THREADS);
    return EXIT_FAILURE;
  }

  if (argc < 2)
  {
    // After parsing the command-line options we expect two additional
    // arguments, the rules file and the target file, directory or pid to
    // be scanned.

    fprintf(stderr, "yara: wrong number of arguments\n");
    fprintf(stderr, "%s\n\n", USAGE_STRING);
    fprintf(stderr, "Try `--help` for more options\n");

    return EXIT_FAILURE;
  }

#if defined(_WIN32) && defined(_UNICODE)
  // In Windows set stdout to UTF-8 mode.
  if (_setmode(_fileno(stdout), _O_U8TEXT) == -1)
  {
    return EXIT_FAILURE;
  }
#endif

  if (!load_modules_data())
    exit_with_code(EXIT_FAILURE);

  result = yr_initialize();

  if (result != ERROR_SUCCESS)
  {
    fprintf(stderr, "error: initialization error (%d)\n", result);
    exit_with_code(EXIT_FAILURE);
  }

  yr_set_configuration(YR_CONFIG_STACK_SIZE, &stack_size);
  yr_set_configuration(YR_CONFIG_MAX_STRINGS_PER_RULE, &max_strings_per_rule);
  yr_set_configuration(
      YR_CONFIG_MAX_PROCESS_MEMORY_CHUNK, &max_process_memory_chunk);

  // Try to load the rules file as a binary file containing
  // compiled rules first

  if (rules_are_compiled)
  {
    // When a binary file containing compiled rules is provided, yara accepts
    // only two arguments, the compiled rules file and the target to be scanned.

    if (argc != 2)
    {
      fprintf(
          stderr,
          "error: can't accept multiple rules files if one of them is in "
          "compiled form.\n");
      exit_with_code(EXIT_FAILURE);
    }

    // Not using yr_rules_load because it does not have support for unicode
    // file names. Instead use open _tfopen for openning the file and
    // yr_rules_load_stream for loading the rules from it.

    FILE* fh = _tfopen(argv[0], _T("rb"));

    if (fh != NULL)
    {
      YR_STREAM stream;

      stream.user_data = fh;
      stream.read = (YR_STREAM_READ_FUNC) fread;

      result = yr_rules_load_stream(&stream, &rules);

      fclose(fh);

      if (result == ERROR_SUCCESS)
        result = define_external_variables(ext_vars, rules, NULL);
    }
    else
    {
      result = ERROR_COULD_NOT_OPEN_FILE;
    }
  }
  else
  {
    // Rules file didn't contain compiled rules, let's handle it
    // as a text file containing rules in source form.

    if (yr_compiler_create(&compiler) != ERROR_SUCCESS)
      exit_with_code(EXIT_FAILURE);

    result = define_external_variables(ext_vars, NULL, compiler);

    if (result != ERROR_SUCCESS)
    {
      print_error(result);
      exit_with_code(EXIT_FAILURE);
    }

    if (atom_quality_table != NULL)
    {
      result = yr_compiler_load_atom_quality_table(
          compiler, atom_quality_table, 0);

      if (result != ERROR_SUCCESS)
      {
        fprintf(stderr, "error loading atom quality table: ");
        print_error(result);
        exit_with_code(EXIT_FAILURE);
      }
    }

    cr.errors = 0;
    cr.warnings = 0;

    yr_compiler_set_callback(compiler, print_compiler_error, &cr);

    if (!compile_files(compiler, argc, argv))
      exit_with_code(EXIT_FAILURE);

    if (cr.errors > 0)
      exit_with_code(EXIT_FAILURE);

    if (fail_on_warnings && cr.warnings > 0)
      exit_with_code(EXIT_FAILURE);

    result = yr_compiler_get_rules(compiler, &rules);

    yr_compiler_destroy(compiler);

    compiler = NULL;
  }

  if (result != ERROR_SUCCESS)
  {
    print_error(result);
    exit_with_code(EXIT_FAILURE);
  }

  if (show_stats)
    print_rules_stats(rules);

  cli_mutex_init(&output_mutex);

  if (fast_scan)
    flags |= SCAN_FLAGS_FAST_MODE;

  scan_opts.deadline = time(NULL) + timeout;

  arg_is_dir = is_directory(argv[argc - 1]);

  if (scan_list_search && arg_is_dir)
  {
    fprintf(stderr, "error: cannot use a directory as scan list.\n");
    exit_with_code(EXIT_FAILURE);
  }
  else if (scan_list_search || arg_is_dir)
  {
    if (file_queue_init() != 0)
    {
      print_error(ERROR_INTERNAL_FATAL_ERROR);
      exit_with_code(EXIT_FAILURE);
    }

    THREAD thread[YR_MAX_THREADS];
    THREAD_ARGS thread_args[YR_MAX_THREADS];

    for (int i = 0; i < threads; i++)
    {
      thread_args[i].deadline = scan_opts.deadline;
      thread_args[i].current_count = 0;

      result = yr_scanner_create(rules, &thread_args[i].scanner);

      if (result != ERROR_SUCCESS)
      {
        print_error(result);
        exit_with_code(EXIT_FAILURE);
      }

      yr_scanner_set_callback(
          thread_args[i].scanner, callback, &thread_args[i].callback_args);

      yr_scanner_set_flags(thread_args[i].scanner, flags);

      if (cli_create_thread(
              &thread[i], scanning_thread, (void*) &thread_args[i]))
      {
        print_error(ERROR_COULD_NOT_CREATE_THREAD);
        exit_with_code(EXIT_FAILURE);
      }
    }

    if (arg_is_dir)
    {
      scan_dir(argv[argc - 1], &scan_opts);
    }
    else
    {
      result = populate_scan_list(argv[argc - 1], &scan_opts);

      if (result != ERROR_SUCCESS)
        exit_with_code(EXIT_FAILURE);
    }

    file_queue_finish();

    // Wait for scan threads to finish
    for (int i = 0; i < threads; i++) cli_thread_join(&thread[i]);

    for (int i = 0; i < threads; i++)
      yr_scanner_destroy(thread_args[i].scanner);

    file_queue_destroy();
  }
  else
  {
    CALLBACK_ARGS user_data = {argv[argc - 1], 0};

    result = yr_scanner_create(rules, &scanner);

    if (result != ERROR_SUCCESS)
    {
      _ftprintf(stderr, _T("error: %d\n"), result);
      exit_with_code(EXIT_FAILURE);
    }

    yr_scanner_set_callback(scanner, callback, &user_data);
    yr_scanner_set_flags(scanner, flags);
    yr_scanner_set_timeout(scanner, timeout);

    // Assume the last argument is a file first. This assures we try to process
    // files that start with numbers first.
    result = scan_file(scanner, argv[argc - 1]);

    if (result == ERROR_COULD_NOT_OPEN_FILE)
    {
      // Is it a PID? To be a PID it must be made up entirely of digits.
      char* endptr = NULL;
      long pid = _tcstol(argv[argc - 1], &endptr, 10);

      if (pid > 0 && argv[argc - 1] != NULL && *endptr == '\x00')
        result = yr_scanner_scan_proc(scanner, (int) pid);
    }

    if (result != ERROR_SUCCESS)
    {
      _ftprintf(stderr, _T("error scanning %s: "), argv[argc - 1]);
      print_scanner_error(scanner, result);
      exit_with_code(EXIT_FAILURE);
    }

    if (print_count_only)
      _tprintf(_T("%d\n"), user_data.current_count);

#ifdef YR_PROFILING_ENABLED
    yr_scanner_print_profiling_info(scanner);
#endif
  }

  result = EXIT_SUCCESS;

_exit:

  unload_modules_data();

  if (scanner != NULL)
    yr_scanner_destroy(scanner);

  if (compiler != NULL)
    yr_compiler_destroy(compiler);

  if (rules != NULL)
    yr_rules_destroy(rules);

  yr_finalize();

  args_free(options);

  return result;
}