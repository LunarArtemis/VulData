enum SCP_SERVER_STATES_E
scp_v0s_accept(struct SCP_CONNECTION *c, struct SCP_SESSION **s, int skipVchk)
{
    tui32 version = 0;
    tui32 size;
    struct SCP_SESSION *session = 0;
    tui16 sz;
    tui32 code = 0;
    char buf[257];

    if (!skipVchk)
    {
        LOG_DBG("[v0:%d] starting connection", __LINE__);

        if (0 == scp_tcp_force_recv(c->in_sck, c->in_s->data, 8))
        {
            c->in_s->end = c->in_s->data + 8;
            in_uint32_be(c->in_s, version);

            if (version != 0)
            {
                log_message(LOG_LEVEL_WARNING, "[v0:%d] connection aborted: version error", __LINE__);
                return SCP_SERVER_STATE_VERSION_ERR;
            }
        }
        else
        {
            log_message(LOG_LEVEL_WARNING, "[v0:%d] connection aborted: network error", __LINE__);
            return SCP_SERVER_STATE_NETWORK_ERR;
        }
    }

    in_uint32_be(c->in_s, size);

    init_stream(c->in_s, 8196);

    if (0 != scp_tcp_force_recv(c->in_sck, c->in_s->data, size - 8))
    {
        log_message(LOG_LEVEL_WARNING, "[v0:%d] connection aborted: network error", __LINE__);
        return SCP_SERVER_STATE_NETWORK_ERR;
    }

    c->in_s->end = c->in_s->data + (size - 8);

    in_uint16_be(c->in_s, code);

    if (code == 0 || code == 10 || code == 20)
    {
        session = scp_session_create();

        if (0 == session)
        {
            log_message(LOG_LEVEL_WARNING, "[v0:%d] connection aborted: network error", __LINE__);
            return SCP_SERVER_STATE_INTERNAL_ERR;
        }

        scp_session_set_version(session, version);

        if (code == 0)
        {
            scp_session_set_type(session, SCP_SESSION_TYPE_XVNC);
        }
        else if (code == 10)
        {
            scp_session_set_type(session, SCP_SESSION_TYPE_XRDP);
        }
        else if (code == 20)
        {
            scp_session_set_type(session, SCP_SESSION_TYPE_XORG);
        }

        /* reading username */
        in_uint16_be(c->in_s, sz);
        buf[sz] = '\0';
        in_uint8a(c->in_s, buf, sz);

        if (0 != scp_session_set_username(session, buf))
        {
            scp_session_destroy(session);
            log_message(LOG_LEVEL_WARNING, "[v0:%d] connection aborted: error setting username", __LINE__);
            return SCP_SERVER_STATE_INTERNAL_ERR;
        }

        /* reading password */
        in_uint16_be(c->in_s, sz);
        buf[sz] = '\0';
        in_uint8a(c->in_s, buf, sz);

        if (0 != scp_session_set_password(session, buf))
        {
            scp_session_destroy(session);
            log_message(LOG_LEVEL_WARNING, "[v0:%d] connection aborted: error setting password", __LINE__);
            return SCP_SERVER_STATE_INTERNAL_ERR;
        }

        /* width */
        in_uint16_be(c->in_s, sz);
        scp_session_set_width(session, sz);
        /* height */
        in_uint16_be(c->in_s, sz);
        scp_session_set_height(session, sz);
        /* bpp */
        in_uint16_be(c->in_s, sz);
        if (0 != scp_session_set_bpp(session, (tui8)sz))
        {
            scp_session_destroy(session);
            log_message(LOG_LEVEL_WARNING,
                        "[v0:%d] connection aborted: unsupported bpp: %d",
                        __LINE__, (tui8)sz);
            return SCP_SERVER_STATE_INTERNAL_ERR;
        }

        if (s_check_rem(c->in_s, 2))
        {
            /* reading domain */
            in_uint16_be(c->in_s, sz);

            if (sz > 0)
            {
                in_uint8a(c->in_s, buf, sz);
                buf[sz] = '\0';
                scp_session_set_domain(session, buf);
            }
        }

        if (s_check_rem(c->in_s, 2))
        {
            /* reading program */
            in_uint16_be(c->in_s, sz);

            if (sz > 0)
            {
                in_uint8a(c->in_s, buf, sz);
                buf[sz] = '\0';
                scp_session_set_program(session, buf);
            }
        }

        if (s_check_rem(c->in_s, 2))
        {
            /* reading directory */
            in_uint16_be(c->in_s, sz);

            if (sz > 0)
            {
                in_uint8a(c->in_s, buf, sz);
                buf[sz] = '\0';
                scp_session_set_directory(session, buf);
            }
        }

        if (s_check_rem(c->in_s, 2))
        {
            /* reading client IP address */
            in_uint16_be(c->in_s, sz);

            if (sz > 0)
            {
                in_uint8a(c->in_s, buf, sz);
                buf[sz] = '\0';
                scp_session_set_client_ip(session, buf);
            }
        }
    }
    else if (code == SCP_GW_AUTHENTICATION)
    {
        /* g_writeln("Command is SCP_GW_AUTHENTICATION"); */
        session = scp_session_create();

        if (0 == session)
        {
            /* until syslog merge log_message(s_log, LOG_LEVEL_WARNING, "[v0:%d] connection aborted: network error",      __LINE__);*/
            return SCP_SERVER_STATE_INTERNAL_ERR;
        }

        scp_session_set_version(session, version);
        scp_session_set_type(session, SCP_GW_AUTHENTICATION);
        /* reading username */
        in_uint16_be(c->in_s, sz);
        buf[sz] = '\0';
        in_uint8a(c->in_s, buf, sz);

        /* g_writeln("Received user name: %s",buf); */
        if (0 != scp_session_set_username(session, buf))
        {
            scp_session_destroy(session);
            /* until syslog merge log_message(s_log, LOG_LEVEL_WARNING, "[v0:%d] connection aborted: error setting        username", __LINE__);*/
            return SCP_SERVER_STATE_INTERNAL_ERR;
        }

        /* reading password */
        in_uint16_be(c->in_s, sz);
        buf[sz] = '\0';
        in_uint8a(c->in_s, buf, sz);

        /* g_writeln("Received password: %s",buf); */
        if (0 != scp_session_set_password(session, buf))
        {
            scp_session_destroy(session);
            /* until syslog merge log_message(s_log, LOG_LEVEL_WARNING, "[v0:%d] connection aborted: error setting password", __LINE__); */
            return SCP_SERVER_STATE_INTERNAL_ERR;
        }
    }
    else
    {
        log_message(LOG_LEVEL_WARNING, "[v0:%d] connection aborted: sequence error", __LINE__);
        return SCP_SERVER_STATE_SEQUENCE_ERR;
    }

    (*s) = session;
    return SCP_SERVER_STATE_OK;
}