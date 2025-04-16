static void proxy_process_command(conn *c, char *command, size_t cmdlen, bool multiget) {
    assert(c != NULL);
    LIBEVENT_THREAD *thr = c->thread;
    struct proxy_hook *hooks = thr->proxy_hooks;
    lua_State *L = thr->L;
    proxy_ctx_t *ctx = thr->proxy_ctx;
    mcp_parser_t pr = {0};

    // Avoid doing resp_start() here, instead do it a bit later or as-needed.
    // This allows us to hop over to the internal text protocol parser, which
    // also calls resp_start().
    // Tighter integration later should obviate the need for this, it is not a
    // permanent solution.
    int ret = process_request(&pr, command, cmdlen);
    if (ret != 0) {
        WSTAT_INCR(c->thread, proxy_conn_errors, 1);
        if (!resp_start(c)) {
            conn_set_state(c, conn_closing);
            return;
        }
        proxy_out_errstring(c->resp, PROXY_CLIENT_ERROR, "parsing request");
        if (ret == -2) {
            // Kill connection on more critical parse failure.
            conn_set_state(c, conn_closing);
        }
        return;
    }

    struct proxy_hook *hook = &hooks[pr.command];
    int hook_ref = hook->lua_ref;
    // if client came from a tagged listener, scan for a more specific hook.
    // TODO: (v2) avoiding a hash table lookup here, but maybe some other
    // datastructure would suffice. for 4-8 tags this is perfectly fast.
    if (c->tag && hook->tagged) {
        struct proxy_hook_tagged *pht = hook->tagged;
        while (pht->lua_ref) {
            if (c->tag == pht->tag) {
                hook_ref = pht->lua_ref;
                break;
            }
            pht++;
        }
    }

    if (!hook_ref) {
        // need to pass our command string into the internal handler.
        // to minimize the code change, this means allowing it to tokenize the
        // full command. The proxy's indirect parser should be built out to
        // become common code for both proxy and ascii handlers.
        // For now this means we have to null-terminate the command string,
        // then call into text protocol handler.
        // FIXME (v2): use a ptr or something; don't like this code.
        if (cmdlen > 1 && command[cmdlen-2] == '\r') {
            command[cmdlen-2] = '\0';
        } else {
            command[cmdlen-1] = '\0';
        }
        // lets nread_proxy know we're in ascii mode.
        c->proxy_coro_ref = 0;
        process_command_ascii(c, command);
        return;
    }

    // If ascii multiget, we turn this into a self-calling loop :(
    // create new request with next key, call this func again, then advance
    // original string.
    // might be better to split this function; the below bits turn into a
    // function call, then we don't re-process the above bits in the same way?
    // The way this is detected/passed on is very fragile.
    if (!multiget && pr.cmd_type == CMD_TYPE_GET && pr.has_space) {
        uint32_t keyoff = pr.tokens[pr.keytoken];
        while (pr.klen != 0) {
            char temp[KEY_MAX_LENGTH + 30];
            char *cur = temp;
            // Core daemon can abort the entire command if one key is bad, but
            // we cannot from the proxy. Instead we have to inject errors into
            // the stream. This should, thankfully, be rare at least.
            if (pr.klen > KEY_MAX_LENGTH) {
                if (!resp_start(c)) {
                    conn_set_state(c, conn_closing);
                    return;
                }
                proxy_out_errstring(c->resp, PROXY_CLIENT_ERROR, "key too long");
            } else {
                // copy original request up until the original key token.
                memcpy(cur, pr.request, pr.tokens[pr.keytoken]);
                cur += pr.tokens[pr.keytoken];

                // now copy in our "current" key.
                memcpy(cur, &pr.request[keyoff], pr.klen);
                cur += pr.klen;

                memcpy(cur, "\r\n", 2);
                cur += 2;

                *cur = '\0';
                P_DEBUG("%s: new multiget sub request: %s [%u/%u]\n", __func__, temp, keyoff, pr.klen);
                proxy_process_command(c, temp, cur - temp, PROCESS_MULTIGET);
            }

            // now advance to the next key.
            keyoff = _process_request_next_key(&pr);
        }

        if (!resp_start(c)) {
            conn_set_state(c, conn_closing);
            return;
        }

        // The above recursions should have created c->resp's in dispatch
        // order.
        // So now we add another one at the end to create the capping END
        // string.
        memcpy(c->resp->wbuf, ENDSTR, ENDLEN);
        resp_add_iov(c->resp, c->resp->wbuf, ENDLEN);

        return;
    }

    // We test the command length all the way down here because multigets can
    // be very long, and they're chopped up by now.
    if (cmdlen >= MCP_REQUEST_MAXLEN) {
        WSTAT_INCR(c->thread, proxy_conn_errors, 1);
        if (!resp_start(c)) {
            conn_set_state(c, conn_closing);
            return;
        }
        proxy_out_errstring(c->resp, PROXY_CLIENT_ERROR, "request too long");
        conn_set_state(c, conn_closing);
        return;
    }

    if (!resp_start(c)) {
        conn_set_state(c, conn_closing);
        return;
    }

    // Count requests handled by proxy vs local.
    // Also batch the counts down this far so we can lock once for the active
    // counter instead of twice.
    struct proxy_int_stats *istats = c->thread->proxy_int_stats;
    uint64_t active_reqs = 0;
    WSTAT_L(c->thread);
    istats->counters[pr.command]++;
    c->thread->stats.proxy_conn_requests++;
    c->thread->stats.proxy_req_active++;
    active_reqs = c->thread->stats.proxy_req_active;
    WSTAT_UL(c->thread);

    if (active_reqs > ctx->active_req_limit) {
        proxy_out_errstring(c->resp, PROXY_SERVER_ERROR, "active request limit reached");
        WSTAT_DECR(c->thread, proxy_req_active, 1);
        if (pr.vlen != 0) {
            c->sbytes = pr.vlen;
            conn_set_state(c, conn_swallow);
        }
        return;
    }

    // start a coroutine.
    // TODO (v2): This can pull a thread from a cache.
    lua_newthread(L);
    lua_State *Lc = lua_tothread(L, -1);
    // leave the thread first on the stack, so we can reference it if needed.
    // pull the lua hook function onto the stack.
    lua_rawgeti(Lc, LUA_REGISTRYINDEX, hook_ref);

    mcp_request_t *rq = mcp_new_request(Lc, &pr, command, cmdlen);
    rq->ascii_multiget = multiget;
    // NOTE: option 1) copy c->tag into rq->tag here.
    // add req:listen_tag() to retrieve in top level route.

    // TODO (v2): lift this to a post-processor?
    if (rq->pr.vlen != 0) {
        c->item = NULL;
        // Need to add the used memory later due to needing an extra callback
        // handler on error during nread.
        bool oom = proxy_bufmem_checkadd(c->thread, 0);

        // relying on temporary malloc's not having fragmentation
        if (!oom) {
            c->item = malloc(rq->pr.vlen);
        }
        if (c->item == NULL) {
            lua_settop(L, 0);
            proxy_out_errstring(c->resp, PROXY_SERVER_ERROR, "out of memory");
            WSTAT_DECR(c->thread, proxy_req_active, 1);
            c->sbytes = rq->pr.vlen;
            conn_set_state(c, conn_swallow);
            return;
        }
        c->item_malloced = true;
        c->ritem = c->item;
        c->rlbytes = rq->pr.vlen;
        c->proxy_coro_ref = luaL_ref(L, LUA_REGISTRYINDEX); // pops coroutine.

        conn_set_state(c, conn_nread);
        return;
    } else {
        conn_set_state(c, conn_new_cmd);
    }

    proxy_run_coroutine(Lc, c->resp, NULL, c);

    lua_settop(L, 0); // clear anything remaining on the main thread.
}