int _ParseHeader(HTTPReq *hr) {
	SOCKET clisock = hr->clisock;
	HTTPReqMessage *req = &(hr->req);
	int n;
	int l, end;
	int i = 0;
	char *p;

	DebugMsg("\tParse Header\n");
	p = (char *)req->_buf;
	memset(p, 0, MAX_HEADER_SIZE + MAX_BODY_SIZE);
	/* GET, PUT ... and a white space are 3 charaters. */
	n = recv(clisock, p, 3, 0);
	if(n == 3) {
		/* Parse method. */
		for(i = 3; n>0; i++) {
			n = recv(clisock, p + i, 1, 0);
			if(n == -EAGAIN || n == -EWOULDBLOCK) {
				n = 1;
				i--;
				continue;
			} else if (n <= 0) {
				return 0;
			}

			if(p[i] == ' ') {
				p[i] = '\0';
				i += 1;
				break;
			}

			if(i > MAX_HTTP_METHOD_LEN - 2)
				return 0;
		}
		req->Header.Method = HaveMethod(p);

		/* Parse URI. */
		req->Header.URI = p + i;
		for(; n>0; i++) {
			n = recv(clisock, p + i, 1, 0);
			if(p[i] == ' ') {
				p[i] = '\0';
				break;
			}
		}

		/* Parse HTTP version. */
		if(n > 0) i += 1;
		req->Header.Version = p + i;
		/* HTTP/1.1 has 8 charaters. */
		n = recv(clisock, p + i, 8, 0);
		for(i+=8; (n>0) && (i<MAX_HEADER_SIZE); i++) {
			n = recv(clisock, p + i, 1, 0);
			if((l = _CheckLine(p + i))) {
				if(l == 2) p[i - 1] = '\0';
				p[i] = '\0';
				break;
			}
		}

		/* Parse other fields. */
		if(n > 0) i += 1;
		req->Header.Fields[req->Header.Amount].key = p + i;
		end = 0;
		for(; (n>0) && (i<MAX_HEADER_SIZE) && (req->Header.Amount<MAX_HEADER_FIELDS); i++) {
			n = recv(clisock, p + i, 1, 0);
			/* Check field key name end. */
			if((l = _CheckFieldSep(p + i))) {
				p[i - 1] = '\0';
				req->Header.Fields[req->Header.Amount].value = p + i + 1;
			}

			/* Check header end. */
			if((l = _CheckLine(p + i))) {
				if(end == 0) {
					if(l == 2) p[i - 1] = '\0';
					p[i] = '\0';

					/* CRLF have 2 characters, so check 2 times new line. */
					end = 2;

					/* Go to parse next header field. */
					req->Header.Amount += 1;
					req->Header.Fields[req->Header.Amount].key = p + i + 1;
				}
				else {
					/* Requset message header finished. */
					break;
				}
			}
			else {
				if(end > 0) end -= 1;
			}
		}
	}
	if(n < 0) {
		hr->work_state = CLOSE_SOCKET;
	}

	req->_index = (n > 0) ? i + 1: i;
	return i;
}