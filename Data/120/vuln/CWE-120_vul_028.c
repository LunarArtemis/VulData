IdKeyPair UserTerminalRouter::acceptNewConnection() {
  LOG(INFO) << "Listening to id/key FIFO";
  int terminalFd = socketHandler->accept(serverFd);
  if (terminalFd < 0) {
    if (GetErrno() != EAGAIN && GetErrno() != EWOULDBLOCK) {
      FATAL_FAIL(-1);  // STFATAL with the error
    } else {
      return IdKeyPair({"", ""});  // Nothing to accept this time
    }
  }

  LOG(INFO) << "Connected";

  try {
    Packet packet;
    if (!socketHandler->readPacket(terminalFd, &packet)) {
      STFATAL << "Missing user info packet";
    }
    if (packet.getHeader() != TerminalPacketType::TERMINAL_USER_INFO) {
      STFATAL << "Got an invalid packet header: " << int(packet.getHeader());
    }
    TerminalUserInfo tui = stringToProto<TerminalUserInfo>(packet.getPayload());
    tui.set_fd(terminalFd);
    idInfoMap[tui.id()] = tui;
    return IdKeyPair({tui.id(), tui.passkey()});
  } catch (const std::runtime_error &re) {
    STFATAL << "Router can't talk to terminal: " << re.what();
  }

  STFATAL << "Should never get here";
  return IdKeyPair({"", ""});
}