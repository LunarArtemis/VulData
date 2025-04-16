void HttpStreamParser::CalculateResponseBodySize() {
  // Figure how to determine EOF:

  // For certain responses, we know the content length is always 0. From
  // RFC 2616 Section 4.3 Message Body:
  //
  // For response messages, whether or not a message-body is included with
  // a message is dependent on both the request method and the response
  // status code (section 6.1.1). All responses to the HEAD request method
  // MUST NOT include a message-body, even though the presence of entity-
  // header fields might lead one to believe they do. All 1xx
  // (informational), 204 (no content), and 304 (not modified) responses
  // MUST NOT include a message-body. All other responses do include a
  // message-body, although it MAY be of zero length.
  switch (response_->headers->response_code()) {
    // Note that 1xx was already handled earlier.
    case 204:  // No Content
    case 205:  // Reset Content
    case 304:  // Not Modified
      response_body_length_ = 0;
      break;
  }
  if (request_->method == "HEAD")
    response_body_length_ = 0;

  if (response_body_length_ == -1) {
    // "Transfer-Encoding: chunked" trumps "Content-Length: N"
    if (response_->headers->IsChunkEncoded()) {
      chunked_decoder_.reset(new HttpChunkedDecoder());
    } else {
      response_body_length_ = response_->headers->GetContentLength();
      // If response_body_length_ is still -1, then we have to wait
      // for the server to close the connection.
    }
  }
}