std::string TransportUDP::getClientURI()
{
  ROS_ASSERT(!is_server_);

  sockaddr_storage sas;
  socklen_t sas_len = sizeof(sas);
  getpeername(sock_, (sockaddr *)&sas, &sas_len);

  sockaddr_in *sin = (sockaddr_in *)&sas;

  char namebuf[128];
  int port = ntohs(sin->sin_port);
  strcpy(namebuf, inet_ntoa(sin->sin_addr));

  std::string ip = namebuf;
  std::stringstream uri;
  uri << ip << ":" << port;

  return uri.str();
}