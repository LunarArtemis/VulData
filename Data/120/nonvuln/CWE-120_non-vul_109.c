static int callback(
    YR_SCAN_CONTEXT* context,
    int message,
    void* message_data,
    void* user_data)
{
  YR_MODULE_IMPORT* mi;
  YR_STRING* string;
  YR_RULE* rule;
  YR_OBJECT* object;
  MODULE_DATA* module_data;

  switch (message)
  {
  case CALLBACK_MSG_RULE_MATCHING:
  case CALLBACK_MSG_RULE_NOT_MATCHING:
    return handle_message(context, message, (YR_RULE*) message_data, user_data);

  case CALLBACK_MSG_IMPORT_MODULE:

    mi = (YR_MODULE_IMPORT*) message_data;
    module_data = modules_data_list;

    while (module_data != NULL)
    {
      if (strcmp(module_data->module_name, mi->module_name) == 0)
      {
        mi->module_data = (void*) module_data->mapped_file.data;
        mi->module_data_size = module_data->mapped_file.size;
        break;
      }

      module_data = module_data->next;
    }

    return CALLBACK_CONTINUE;

  case CALLBACK_MSG_MODULE_IMPORTED:

    if (show_module_data)
    {
      object = (YR_OBJECT*) message_data;

      cli_mutex_lock(&output_mutex);

#if defined(_WIN32)
      // In Windows restore stdout to normal text mode as yr_object_print_data
      // calls printf which is not supported in UTF-8 mode.
      _setmode(_fileno(stdout), _O_TEXT);
#endif

      yr_object_print_data(object, 0, 1);
      printf("\n");

#if defined(_WIN32)
      // Go back to UTF-8 mode.
      _setmode(_fileno(stdout), _O_U8TEXT);
#endif

      cli_mutex_unlock(&output_mutex);
    }

    return CALLBACK_CONTINUE;

  case CALLBACK_MSG_TOO_MANY_MATCHES:
    if (ignore_warnings)
      return CALLBACK_CONTINUE;

    string = (YR_STRING*) message_data;
    rule = &context->rules->rules_table[string->rule_idx];

    fprintf(
        stderr,
        "warning: rule \"%s\": too many matches for %s, results for this rule "
        "may be incorrect\n",
        rule->identifier,
        string->identifier);

    if (fail_on_warnings)
      return CALLBACK_ERROR;

    return CALLBACK_CONTINUE;
  }

  return CALLBACK_ERROR;
}