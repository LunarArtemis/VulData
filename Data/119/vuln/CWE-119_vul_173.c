STDMETHODIMP Hook_Terminate(InternetProtocol_Terminate_Fn orig_req,
                            IInternetProtocol* protocol,
                            DWORD options) {
  scoped_refptr<ProtData> prot_data = ProtData::DataFromProtocol(protocol);
  // TODO(ananta/robertshield)
  // Write a test for this bug http://crbug.com/178421
  if (prot_data && !IsChrome(prot_data->renderer_type()))
    prot_data->Invalidate();

  // We are just pass through at this point, avoid false positive crash
  // reports.
  ExceptionBarrierReportOnlyModule barrier;
  return orig_req(protocol, options);
}