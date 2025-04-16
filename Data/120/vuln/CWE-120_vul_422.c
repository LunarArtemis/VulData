static int try_read_command_binary(conn *c) {
    /* Do we have the complete packet header? */
    if (c->rbytes < sizeof(c->binary_header)) {
        /* need more data! */
        return 0;
    } else {
        memcpy(&c->binary_header, c->rcurr, sizeof(c->binary_header));
        protocol_binary_request_header* req;
        req = &c->binary_header;

        if (settings.verbose > 1) {
            /* Dump the packet before we convert it to host order */
            int ii;
            fprintf(stderr, "<%d Read binary protocol data:", c->sfd);
            for (ii = 0; ii < sizeof(req->bytes); ++ii) {
                if (ii % 4 == 0) {
                    fprintf(stderr, "\n<%d   ", c->sfd);
                }
                fprintf(stderr, " 0x%02x", req->bytes[ii]);
            }
            fprintf(stderr, "\n");
        }

        c->binary_header = *req;
        c->binary_header.request.keylen = ntohs(req->request.keylen);
        c->binary_header.request.bodylen = ntohl(req->request.bodylen);
        c->binary_header.request.cas = ntohll(req->request.cas);

        if (c->binary_header.request.magic != PROTOCOL_BINARY_REQ) {
            if (settings.verbose) {
                fprintf(stderr, "Invalid magic:  %x\n",
                        c->binary_header.request.magic);
            }
            conn_set_state(c, conn_closing);
            return -1;
        }

        uint8_t extlen = c->binary_header.request.extlen;
        uint16_t keylen = c->binary_header.request.keylen;
        if (c->rbytes < keylen + extlen + sizeof(c->binary_header)) {
            // Still need more bytes. Let try_read_network() realign the
            // read-buffer and fetch more data as necessary.
            return 0;
        }

        if (!resp_start(c)) {
            conn_set_state(c, conn_closing);
            return -1;
        }

        c->cmd = c->binary_header.request.opcode;
        c->keylen = c->binary_header.request.keylen;
        c->opaque = c->binary_header.request.opaque;
        /* clear the returned cas value */
        c->cas = 0;

        c->last_cmd_time = current_time;
        // sigh. binprot has no "largest possible extlen" define, and I don't
        // want to refactor a ton of code either. Header is only ever used out
        // of c->binary_header, but the extlen stuff is used for the latter
        // bytes. Just wastes 24 bytes on the stack this way.
        char extbuf[sizeof(c->binary_header) + BIN_MAX_EXTLEN];
        memcpy(extbuf + sizeof(c->binary_header), c->rcurr + sizeof(c->binary_header), extlen);
        c->rbytes -= sizeof(c->binary_header) + extlen + keylen;
        c->rcurr += sizeof(c->binary_header) + extlen + keylen;

        dispatch_bin_command(c, extbuf);
    }

    return 1;
}