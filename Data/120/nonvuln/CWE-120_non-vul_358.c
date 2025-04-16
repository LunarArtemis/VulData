void
ts_output_string (TsOutputType  type,
                  const char   *string,
                  int           len)
{
  if (len < 0)
    len = strlen (string);

  if (ts_output_handler && len > 0)
    (* ts_output_handler) (type, string, len, ts_output_data);
}