de265_error decoder_context::read_pps_NAL(bitreader& reader)
{
  logdebug(LogHeaders,"----> read PPS\n");

  std::shared_ptr<pic_parameter_set> new_pps = std::make_shared<pic_parameter_set>();

  bool success = new_pps->read(&reader,this);

  if (param_pps_headers_fd>=0) {
    new_pps->dump(param_pps_headers_fd);
  }

  if (success) {
    pps[ (int)new_pps->pic_parameter_set_id ] = new_pps;
  }

  return success ? DE265_OK : DE265_WARNING_PPS_HEADER_INVALID;
}