UserTerminalRouter::UserTerminalRouter(
    shared_ptr<PipeSocketHandler> _socketHandler,
    const SocketEndpoint &_routerEndpoint)
    : socketHandler(_socketHandler) {
  serverFd = *(socketHandler->listen(_routerEndpoint).begin());
  FATAL_FAIL(::chown(_routerEndpoint.name().c_str(), getuid(), getgid()));
  FATAL_FAIL(::chmod(_routerEndpoint.name().c_str(),
                     S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP |
                         S_IROTH | S_IWOTH | S_IXOTH));
}