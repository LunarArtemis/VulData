bool TransportTCP::connect(const std::string& host, int port)
{
  if (!isHostAllowed(host))
    return false; // adios amigo

  sock_ = socket(s_use_ipv6_ ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
  connected_host_ = host;
  connected_port_ = port;

  if (sock_ == ROS_INVALID_SOCKET)
  {
    ROS_ERROR("socket() failed with error [%s]",  last_socket_error_string());
    return false;
  }

  setNonBlocking();

  sockaddr_storage sas;
  socklen_t sas_len;

  in_addr ina;
  in6_addr in6a;
  if (inet_pton(AF_INET, host.c_str(), &ina) == 1)
  {
    sockaddr_in *address = (sockaddr_in*) &sas;
    sas_len = sizeof(sockaddr_in);
    
    la_len_ = sizeof(sockaddr_in);
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    address->sin_addr.s_addr = ina.s_addr;
  }
  else if (inet_pton(AF_INET6, host.c_str(), &in6a) == 1)
  {
    sockaddr_in6 *address = (sockaddr_in6*) &sas;
    sas_len = sizeof(sockaddr_in6);
    la_len_ = sizeof(sockaddr_in6);
    address->sin6_family = AF_INET6;
    address->sin6_port = htons(port);
    memcpy(address->sin6_addr.s6_addr, in6a.s6_addr, sizeof(in6a.s6_addr));
  }
  else
  {
    struct addrinfo* addr;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;

    if (getaddrinfo(host.c_str(), NULL, &hints, &addr) != 0)
    {
      close();
      ROS_ERROR("couldn't resolve publisher host [%s]", host.c_str());
      return false;
    }

    bool found = false;
    struct addrinfo* it = addr;
    char namebuf[128];
    for (; it; it = it->ai_next)
    {
      if (!s_use_ipv6_ && it->ai_family == AF_INET)
      {
        sockaddr_in *address = (sockaddr_in*) &sas;
        sas_len = sizeof(*address);
        
        memcpy(address, it->ai_addr, it->ai_addrlen);
        address->sin_family = it->ai_family;
        address->sin_port = htons(port);
	
        strcpy(namebuf, inet_ntoa(address->sin_addr));
        found = true;
        break;
      }
      if (s_use_ipv6_ && it->ai_family == AF_INET6)
      {
        sockaddr_in6 *address = (sockaddr_in6*) &sas;
        sas_len = sizeof(*address);
      
        memcpy(address, it->ai_addr, it->ai_addrlen);
        address->sin6_family = it->ai_family;
        address->sin6_port = htons((u_short) port);
      
        // TODO IPV6: does inet_ntop need other includes for Windows?
        inet_ntop(AF_INET6, (void*)&(address->sin6_addr), namebuf, sizeof(namebuf));
        found = true;
        break;
      }
    }

    freeaddrinfo(addr);

    if (!found)
    {
      ROS_ERROR("Couldn't resolve an address for [%s]\n", host.c_str());
      return false;
    }

    ROSCPP_LOG_DEBUG("Resolved publisher host [%s] to [%s] for socket [%d]", host.c_str(), namebuf, sock_);
  }

  int ret = ::connect(sock_, (sockaddr*) &sas, sas_len);
  // windows might need some time to sleep (input from service robotics hack) add this if testing proves it is necessary.
  // ROS_ASSERT((flags_ & SYNCHRONOUS) || ret != 0);
  if (((flags_ & SYNCHRONOUS) && ret != 0) || // synchronous, connect() should return 0
      (!(flags_ & SYNCHRONOUS) && ret != 0 && last_socket_error() != ROS_SOCKETS_ASYNCHRONOUS_CONNECT_RETURN)) 
      // asynchronous, connect() may return 0 or -1. When return -1, WSAGetLastError()=WSAEWOULDBLOCK/errno=EINPROGRESS
  {
    ROSCPP_CONN_LOG_DEBUG("Connect to tcpros publisher [%s:%d] failed with error [%d, %s]", host.c_str(), port, ret, last_socket_error_string());
    close();

    return false;
  }

  // from daniel stonier:
#ifdef WIN32
  // This is hackish, but windows fails at recv() if its slow to connect (e.g. happens with wireless)
  // recv() needs to check if its connected or not when its asynchronous?
  Sleep(100);
#endif


  std::stringstream ss;
  ss << host << ":" << port << " on socket " << sock_;
  cached_remote_host_ = ss.str();

  if (!initializeSocket())
  {
    return false;
  }

  if (flags_ & SYNCHRONOUS)
  {
    ROSCPP_CONN_LOG_DEBUG("connect() succeeded to [%s:%d] on socket [%d]", host.c_str(), port, sock_);
  }
  else
  {
    ROSCPP_CONN_LOG_DEBUG("Async connect() in progress to [%s:%d] on socket [%d]", host.c_str(), port, sock_);
  }

  return true;
}