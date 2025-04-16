void*
xmlNanoHTTPMethodRedir(const char *URL, const char *method, const char *input,
                  char **contentType, char **redir,
		  const char *headers, int ilen ) {
    xmlNanoHTTPCtxtPtr ctxt;
    char *bp, *p;
    int blen;
    SOCKET ret;
    int nbRedirects = 0;
    char *redirURL = NULL;
#ifdef DEBUG_HTTP
    int xmt_bytes;
#endif

    if (URL == NULL) return(NULL);
    if (method == NULL) method = "GET";
    xmlNanoHTTPInit();

retry:
    if (redirURL == NULL) {
	ctxt = xmlNanoHTTPNewCtxt(URL);
	if (ctxt == NULL)
	    return(NULL);
    } else {
	ctxt = xmlNanoHTTPNewCtxt(redirURL);
	if (ctxt == NULL)
	    return(NULL);
	ctxt->location = xmlMemStrdup(redirURL);
    }

    if ((ctxt->protocol == NULL) || (strcmp(ctxt->protocol, "http"))) {
	__xmlIOErr(XML_FROM_HTTP, XML_HTTP_URL_SYNTAX, "Not a valid HTTP URI");
        xmlNanoHTTPFreeCtxt(ctxt);
	if (redirURL != NULL) xmlFree(redirURL);
        return(NULL);
    }
    if (ctxt->hostname == NULL) {
	__xmlIOErr(XML_FROM_HTTP, XML_HTTP_UNKNOWN_HOST,
	           "Failed to identify host in URI");
        xmlNanoHTTPFreeCtxt(ctxt);
	if (redirURL != NULL) xmlFree(redirURL);
        return(NULL);
    }
    if (proxy) {
	blen = strlen(ctxt->hostname) * 2 + 16;
	ret = xmlNanoHTTPConnectHost(proxy, proxyPort);
    }
    else {
	blen = strlen(ctxt->hostname);
	ret = xmlNanoHTTPConnectHost(ctxt->hostname, ctxt->port);
    }
    if (ret == INVALID_SOCKET) {
        xmlNanoHTTPFreeCtxt(ctxt);
	if (redirURL != NULL) xmlFree(redirURL);
        return(NULL);
    }
    ctxt->fd = ret;

    if (input == NULL)
	ilen = 0;
    else
	blen += 36;

    if (headers != NULL)
	blen += strlen(headers) + 2;
    if (contentType && *contentType)
	/* reserve for string plus 'Content-Type: \r\n" */
	blen += strlen(*contentType) + 16;
    if (ctxt->query != NULL)
	/* 1 for '?' */
	blen += strlen(ctxt->query) + 1;
    blen += strlen(method) + strlen(ctxt->path) + 24;
#ifdef HAVE_ZLIB_H
    /* reserve for possible 'Accept-Encoding: gzip' string */
    blen += 23;
#endif
    if (ctxt->port != 80) {
	/* reserve space for ':xxxxx', incl. potential proxy */
	if (proxy)
	    blen += 12;
	else
	    blen += 6;
    }
    bp = (char*)xmlMallocAtomic(blen);
    if ( bp == NULL ) {
        xmlNanoHTTPFreeCtxt( ctxt );
	xmlHTTPErrMemory("allocating header buffer");
	return ( NULL );
    }

    p = bp;

    if (proxy) {
	if (ctxt->port != 80) {
	    p += snprintf( p, blen - (p - bp), "%s http://%s:%d%s",
			method, ctxt->hostname,
			ctxt->port, ctxt->path );
	}
	else
	    p += snprintf( p, blen - (p - bp), "%s http://%s%s", method,
			ctxt->hostname, ctxt->path);
    }
    else
	p += snprintf( p, blen - (p - bp), "%s %s", method, ctxt->path);

    if (ctxt->query != NULL)
	p += snprintf( p, blen - (p - bp), "?%s", ctxt->query);

    if (ctxt->port == 80) {
        p += snprintf( p, blen - (p - bp), " HTTP/1.0\r\nHost: %s\r\n",
		    ctxt->hostname);
    } else {
        p += snprintf( p, blen - (p - bp), " HTTP/1.0\r\nHost: %s:%d\r\n",
		    ctxt->hostname, ctxt->port);
    }

#ifdef HAVE_ZLIB_H
    p += snprintf(p, blen - (p - bp), "Accept-Encoding: gzip\r\n");
#endif

    if (contentType != NULL && *contentType)
	p += snprintf(p, blen - (p - bp), "Content-Type: %s\r\n", *contentType);

    if (headers != NULL)
	p += snprintf( p, blen - (p - bp), "%s", headers );

    if (input != NULL)
	snprintf(p, blen - (p - bp), "Content-Length: %d\r\n\r\n", ilen );
    else
	snprintf(p, blen - (p - bp), "\r\n");

#ifdef DEBUG_HTTP
    xmlGenericError(xmlGenericErrorContext,
	    "-> %s%s", proxy? "(Proxy) " : "", bp);
    if ((blen -= strlen(bp)+1) < 0)
	xmlGenericError(xmlGenericErrorContext,
		"ERROR: overflowed buffer by %d bytes\n", -blen);
#endif
    ctxt->outptr = ctxt->out = bp;
    ctxt->state = XML_NANO_HTTP_WRITE;
    blen = strlen( ctxt->out );
#ifdef DEBUG_HTTP
    xmt_bytes = xmlNanoHTTPSend(ctxt, ctxt->out, blen );
    if ( xmt_bytes != blen )
        xmlGenericError( xmlGenericErrorContext,
			"xmlNanoHTTPMethodRedir:  Only %d of %d %s %s\n",
			xmt_bytes, blen,
			"bytes of HTTP headers sent to host",
			ctxt->hostname );
#else
    xmlNanoHTTPSend(ctxt, ctxt->out, blen );
#endif

    if ( input != NULL ) {
#ifdef DEBUG_HTTP
        xmt_bytes = xmlNanoHTTPSend( ctxt, input, ilen );

	if ( xmt_bytes != ilen )
	    xmlGenericError( xmlGenericErrorContext,
			"xmlNanoHTTPMethodRedir:  Only %d of %d %s %s\n",
			xmt_bytes, ilen,
			"bytes of HTTP content sent to host",
			ctxt->hostname );
#else
	xmlNanoHTTPSend( ctxt, input, ilen );
#endif
    }

    ctxt->state = XML_NANO_HTTP_READ;

    while ((p = xmlNanoHTTPReadLine(ctxt)) != NULL) {
        if (*p == 0) {
	    ctxt->content = ctxt->inrptr;
	    xmlFree(p);
	    break;
	}
	xmlNanoHTTPScanAnswer(ctxt, p);

#ifdef DEBUG_HTTP
	xmlGenericError(xmlGenericErrorContext, "<- %s\n", p);
#endif
        xmlFree(p);
    }

    if ((ctxt->location != NULL) && (ctxt->returnValue >= 300) &&
        (ctxt->returnValue < 400)) {
#ifdef DEBUG_HTTP
	xmlGenericError(xmlGenericErrorContext,
		"\nRedirect to: %s\n", ctxt->location);
#endif
	while ( xmlNanoHTTPRecv(ctxt) > 0 ) ;
        if (nbRedirects < XML_NANO_HTTP_MAX_REDIR) {
	    nbRedirects++;
	    if (redirURL != NULL)
		xmlFree(redirURL);
	    redirURL = xmlMemStrdup(ctxt->location);
	    xmlNanoHTTPFreeCtxt(ctxt);
	    goto retry;
	}
	xmlNanoHTTPFreeCtxt(ctxt);
	if (redirURL != NULL) xmlFree(redirURL);
#ifdef DEBUG_HTTP
	xmlGenericError(xmlGenericErrorContext,
		"xmlNanoHTTPMethodRedir: Too many redirects, aborting ...\n");
#endif
	return(NULL);
    }

    if (contentType != NULL) {
	if (ctxt->contentType != NULL)
	    *contentType = xmlMemStrdup(ctxt->contentType);
	else
	    *contentType = NULL;
    }

    if ((redir != NULL) && (redirURL != NULL)) {
	*redir = redirURL;
    } else {
	if (redirURL != NULL)
	    xmlFree(redirURL);
	if (redir != NULL)
	    *redir = NULL;
    }

#ifdef DEBUG_HTTP
    if (ctxt->contentType != NULL)
	xmlGenericError(xmlGenericErrorContext,
		"\nCode %d, content-type '%s'\n\n",
	       ctxt->returnValue, ctxt->contentType);
    else
	xmlGenericError(xmlGenericErrorContext,
		"\nCode %d, no content-type\n\n",
	       ctxt->returnValue);
#endif

    return((void *) ctxt);
}