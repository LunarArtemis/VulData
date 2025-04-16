int uev_run(uev_ctx_t *ctx, int flags)
{
	int timeout = -1;
	uev_t *w;

        if (!ctx || ctx->fd < 0) {
		errno = EINVAL;
                return -1;
	}

	if (flags & UEV_NONBLOCK)
		timeout = 0;

	/* Start the event loop */
	ctx->running = 1;

	/* Start all dormant timers */
	_UEV_FOREACH(w, ctx->watchers) {
		if (UEV_CRON_TYPE == w->type)
			uev_cron_set(w, w->u.c.when, w->u.c.interval);
		if (UEV_TIMER_TYPE == w->type)
			uev_timer_set(w, w->u.t.timeout, w->u.t.period);
	}

	while (ctx->running && ctx->watchers) {
		struct epoll_event ee[UEV_MAX_EVENTS];
		int i, nfds, rerun = 0;

		/* Handle special case: `application < file.txt` */
		if (ctx->workaround) {
			_UEV_FOREACH(w, ctx->watchers) {
				if (w->active != -1 || !w->cb)
					continue;

				if (!has_data(w->fd)) {
					w->active = 0;
					_UEV_REMOVE(w, ctx->watchers);
				}

				rerun++;
				w->cb(w, w->arg, UEV_READ);
			}
		}

		if (rerun)
			continue;
		ctx->workaround = 0;

		while ((nfds = epoll_wait(ctx->fd, ee, ctx->maxevents, timeout)) < 0) {
			if (!ctx->running)
				break;

			if (EINTR == errno)
				continue; /* Signalled, try again */

			/* Unrecoverable error, cleanup and exit with error. */
			uev_exit(ctx);

			return -2;
		}

		for (i = 0; ctx->running && i < nfds; i++) {
			struct signalfd_siginfo fdsi;
			ssize_t sz = sizeof(fdsi);
			uint32_t events;
			uint64_t exp;

			w = (uev_t *)ee[i].data.ptr;
			events = ee[i].events;

			switch (w->type) {
			case UEV_IO_TYPE:
				if (events & (EPOLLHUP | EPOLLERR))
					uev_io_stop(w);
				break;

			case UEV_SIGNAL_TYPE:
				if (read(w->fd, &fdsi, sz) != sz) {
					if (uev_signal_start(w)) {
						uev_signal_stop(w);
						events = UEV_ERROR;
					}
					memset(&w->siginfo, 0, sizeof(w->siginfo));
				} else
					w->siginfo = fdsi;
				break;

			case UEV_TIMER_TYPE:
				if (read(w->fd, &exp, sizeof(exp)) != sizeof(exp)) {
					uev_timer_stop(w);
					events = UEV_ERROR;
				}

				if (!w->u.t.period)
					w->u.t.timeout = 0;
				if (!w->u.t.timeout)
					uev_timer_stop(w);
				break;

			case UEV_CRON_TYPE:
				if (read(w->fd, &exp, sizeof(exp)) != sizeof(exp)) {
					events = UEV_HUP;
					if (errno != ECANCELED) {
						uev_cron_stop(w);
						events = UEV_ERROR;
					}
				}

				if (!w->u.c.interval)
					w->u.c.when = 0;
				else
					w->u.c.when += w->u.c.interval;
				if (!w->u.c.when)
					uev_timer_stop(w);
				break;

			case UEV_EVENT_TYPE:
				if (read(w->fd, &exp, sizeof(exp)) != sizeof(exp))
					events = UEV_HUP;
				break;
			}

			/*
			 * NOTE: Must be last action for watcher, the
			 *       callback may delete itself.
			 */
			if (w->cb)
				w->cb(w, w->arg, events & UEV_EVENT_MASK);
		}

		if (flags & UEV_ONCE)
			break;
	}

	return 0;
}