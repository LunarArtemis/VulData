std::string TransportTCP::getClientURI()
{
  ROS_ASSERT(!is_server_);

  sockaddr_storage sas;
  socklen_t sas_len = sizeof(sas);
  getpeername(sock_, (sockaddr *)&sas, &sas_len);
  
  sockaddr_in *sin = (sockaddr_in *)&sas;
  sockaddr_in6 *sin6 = (sockaddr_in6 *)&sas;

  char namebuf[128];
  int port;

  switch (sas.ss_family)
  {
    case AF_INET:
      port = ntohs(sin->sin_port);
      strcpy(namebuf, inet_ntoa(sin->sin_addr));
      break;
    case AF_INET6:
      port = ntohs(sin6->sin6_port);
      inet_ntop(AF_INET6, (void*)&(sin6->sin6_addr), namebuf, sizeof(namebuf));
      break;
    default:
      namebuf[0] = 0;
      port = 0;
      break;
  }

  std::string ip = namebuf;
  std::stringstream uri;
  uri << ip << ":" << port;

  return uri.str();
}