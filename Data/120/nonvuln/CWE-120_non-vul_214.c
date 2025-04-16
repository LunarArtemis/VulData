void PipeSocketHandler::stopListening(const SocketEndpoint& endpoint) {
  lock_guard<std::recursive_mutex> guard(globalMutex);

  string pipePath = endpoint.name();
  auto it = pipeServerSockets.find(pipePath);
  if (it == pipeServerSockets.end()) {
    STFATAL << "Tried to stop listening to a pipe that we weren't listening on:"
            << pipePath;
  }
  int sockFd = *(it->second.begin());
#ifdef _MSC_VER
  FATAL_FAIL(::closesocket(sockFd));
#else
  FATAL_FAIL(::close(sockFd));
#endif
}