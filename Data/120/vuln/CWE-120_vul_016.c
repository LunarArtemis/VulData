int _tmain(int argc, const char_t** argv)
{
  COMPILER_RESULTS cr;

  YR_COMPILER* compiler = NULL;
  YR_RULES* rules = NULL;

  int result;

  argc = args_parse(options, argc, argv);

  if (show_version)
  {
    printf("%s\n", YR_VERSION);
    return EXIT_SUCCESS;
  }

  if (show_help)
  {
    printf("%s\n\n", USAGE_STRING);

    args_print_usage(options, 40);
    printf("\nSend bug reports and suggestions to: vmalvarez@virustotal.com\n");

    return EXIT_SUCCESS;
  }

  if (argc < 2)
  {
    fprintf(stderr, "yarac: wrong number of arguments\n");
    fprintf(stderr, "%s\n\n", USAGE_STRING);
    fprintf(stderr, "Try `--help` for more options\n");

    exit_with_code(EXIT_FAILURE);
  }

  if (yr_initialize() != ERROR_SUCCESS)
    exit_with_code(EXIT_FAILURE);

  if (yr_compiler_create(&compiler) != ERROR_SUCCESS)
    exit_with_code(EXIT_FAILURE);

  if (define_external_variables(ext_vars, NULL, compiler) != ERROR_SUCCESS)
    exit_with_code(EXIT_FAILURE);

  if (atom_quality_table != NULL)
  {
    result = yr_compiler_load_atom_quality_table(
        compiler, atom_quality_table, 0);

    if (result != ERROR_SUCCESS)
    {
      fprintf(stderr, "error loading atom quality table\n");
      exit_with_code(EXIT_FAILURE);
    }
  }

  cr.errors = 0;
  cr.warnings = 0;

  yr_set_configuration(YR_CONFIG_MAX_STRINGS_PER_RULE, &max_strings_per_rule);
  yr_compiler_set_callback(compiler, report_error, &cr);

  if (!compile_files(compiler, argc, argv))
    exit_with_code(EXIT_FAILURE);

  if (cr.errors > 0)
    exit_with_code(EXIT_FAILURE);

  if (fail_on_warnings && cr.warnings > 0)
    exit_with_code(EXIT_FAILURE);

  result = yr_compiler_get_rules(compiler, &rules);

  if (result != ERROR_SUCCESS)
  {
    fprintf(stderr, "error: %d\n", result);
    exit_with_code(EXIT_FAILURE);
  }

  // Not using yr_rules_save because it does not have support for unicode
  // file names. Instead use open _tfopen for openning the file and
  // yr_rules_save_stream for writing the rules to it.

  FILE* fh = _tfopen(argv[argc - 1], _T("wb"));

  if (fh != NULL)
  {
    YR_STREAM stream;

    stream.user_data = fh;
    stream.write = (YR_STREAM_WRITE_FUNC) fwrite;

    result = yr_rules_save_stream(rules, &stream);

    fclose(fh);
  }

  if (result != ERROR_SUCCESS)
  {
    fprintf(stderr, "error: %d\n", result);
    exit_with_code(EXIT_FAILURE);
  }

  result = EXIT_SUCCESS;

_exit:

  if (compiler != NULL)
    yr_compiler_destroy(compiler);

  if (rules != NULL)
    yr_rules_destroy(rules);

  yr_finalize();

  args_free(options);

  return result;
}