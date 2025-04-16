int HttpStreamParser::DoParseResponseHeaders(int end_offset) {
  scoped_refptr<HttpResponseHeaders> headers;
  if (response_header_start_offset_ >= 0) {
    headers = new HttpResponseHeaders(HttpUtil::AssembleRawHeaders(
        read_buf_->StartOfBuffer() + read_buf_unused_offset_, end_offset));
  } else {
    // Enough data was read -- there is no status line.
    headers = new HttpResponseHeaders(std::string("HTTP/0.9 200 OK"));
  }

  // Check for multiple Content-Length headers with no Transfer-Encoding header.
  // If they exist, and have distinct values, it's a potential response
  // smuggling attack.
  if (!headers->HasHeader("Transfer-Encoding")) {
    if (HeadersContainMultipleCopiesOfField(*headers.get(), "Content-Length"))
      return ERR_RESPONSE_HEADERS_MULTIPLE_CONTENT_LENGTH;
  }

  // Check for multiple Content-Disposition or Location headers.  If they exist,
  // it's also a potential response smuggling attack.
  if (HeadersContainMultipleCopiesOfField(*headers.get(),
                                          "Content-Disposition"))
    return ERR_RESPONSE_HEADERS_MULTIPLE_CONTENT_DISPOSITION;
  if (HeadersContainMultipleCopiesOfField(*headers.get(), "Location"))
    return ERR_RESPONSE_HEADERS_MULTIPLE_LOCATION;

  response_->headers = headers;
  response_->connection_info = HttpResponseInfo::CONNECTION_INFO_HTTP1;
  response_->vary_data.Init(*request_, *response_->headers.get());
  DVLOG(1) << __FUNCTION__ << "()"
           << " content_length = \"" << response_->headers->GetContentLength()
           << "\n\""
           << " headers = \""
           << GetResponseHeaderLines(*response_->headers.get()) << "\"";
  return OK;
}