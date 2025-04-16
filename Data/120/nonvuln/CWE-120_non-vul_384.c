void
ts_register_output_func (ts_output_func  func,
                         gpointer        user_data)
{
  ts_output_handler = func;
  ts_output_data    = user_data;
}