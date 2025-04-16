TerminalClient::~TerminalClient() {
  connection->shutdown();
  console.reset();
  portForwardHandler.reset();
  connection.reset();
}