char* url::get_pac() {
	int sock = -1;
	bool chunked = false;
	unsigned long int content_length = 0, status = 0;
	char* buffer = NULL;
	string request;

	// In case of a file:// url we open the file and read it
	if (m_scheme == "file" || m_scheme == "pac+file") {
		struct stat st;
		if ((sock = ::open(m_path.c_str(), O_RDONLY)) < 0)
			return NULL;

		if (!fstat(sock, &st) && st.st_size < PAC_MAX_SIZE) {
			buffer = new char[st.st_size+1];
			memset(buffer, 0, st.st_size+1);
			if (read(sock, buffer, st.st_size) == 0) {
				delete[] buffer;
				buffer = NULL;
			}
		}
		close(sock);
		return buffer;
	}

	// DNS lookup of host
	if (!get_ips(true))
		return NULL;

	// Iterate through each IP trying to make a connection
	// Stop at the first one
	for (int i=0 ; m_ips[i] ; i++) {
		sock = socket(m_ips[i]->sa_family, SOCK_STREAM, 0);
		if (sock < 0) continue;

		if (m_ips[i]->sa_family == AF_INET &&
			!connect(sock, m_ips[i], sizeof(struct sockaddr_in)))
			break;
		else if (m_ips[i]->sa_family == AF_INET6 &&
			!connect(sock, m_ips[i], sizeof(struct sockaddr_in6)))
			break;

		closesocket(sock);
		sock = -1;
	}

	// Test our socket
	if (sock < 0) return NULL;

	// Build the request string
	request  = "GET " + (m_path.size() > 0 ? m_path : "/") + m_query + " HTTP/1.1\r\n";
	request += "Host: " + m_host + "\r\n";
	request += "Accept: " + string(PAC_MIME_TYPE) + "\r\n";
	request += "Connection: close\r\n";
	request += "\r\n";

	// Send HTTP request
	if ((size_t) send(sock, request.c_str(), request.size(), 0) != request.size()) {
		closesocket(sock);
		return NULL;
	}

	/* Verify status line */
	string line = recvline(sock);
	if (sscanf(line.c_str(), "HTTP/1.%*d %lu", &status) == 1 && status == 200) {
		/* Check for correct mime type and content length */
		content_length = 0;
		for (line = recvline(sock) ; line != "\r" && line != "" ; line = recvline(sock)) {
			// Check for chunked encoding
			if (line.find("Content-Transfer-Encoding: chunked") == 0 || line.find("Transfer-Encoding: chunked") == 0)
				chunked = true;

			// Check for content length
			else if (content_length == 0)
				sscanf(line.c_str(), "Content-Length: %lu", &content_length);
		}

		// Get content
		unsigned int recvd = 0;
		buffer = new char[PAC_MAX_SIZE];
		memset(buffer, 0, PAC_MAX_SIZE);
		do {
			unsigned int chunk_length;

			if (chunked) {
				// Discard the empty line if we received a previous chunk
				if (recvd > 0) recvline(sock);

				// Get the chunk-length line as an integer
				if (sscanf(recvline(sock).c_str(), "%x", &chunk_length) != 1 || chunk_length == 0) break;

				// Add this chunk to our content length,
				// ensuring that we aren't over our max size
				content_length += chunk_length;
			}

			if (content_length >= PAC_MAX_SIZE) break;

			while (content_length == 0 || recvd != content_length) {
				int r = recv(sock, buffer + recvd,
				             content_length == 0 ? PAC_HTTP_BLOCK_SIZE
				                                 : content_length - recvd, 0);
				if (r <= 0) {
					chunked = false;
					break;
				}
				recvd += r;
			}
		} while (chunked);

		if (content_length != 0 && string(buffer).size() != content_length) {
			delete[] buffer;
			buffer = NULL;
		}
	}

	// Clean up
	shutdown(sock, SHUT_RDWR);
	closesocket(sock);
	return buffer;
}