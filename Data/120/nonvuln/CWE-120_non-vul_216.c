set<int> PipeSocketHandler::getEndpointFds(const SocketEndpoint& endpoint) {
  lock_guard<std::recursive_mutex> guard(globalMutex);

  string pipePath = endpoint.name();
  if (pipeServerSockets.find(pipePath) == pipeServerSockets.end()) {
    STFATAL << "Tried to getPipeFd on a pipe without calling listen() first: "
            << pipePath;
  }
  return pipeServerSockets[pipePath];
}