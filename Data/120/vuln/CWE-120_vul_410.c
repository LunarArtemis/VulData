static void
_blackbox_vlogger(int32_t target,
		  struct qb_log_callsite *cs, struct timespec *timestamp, va_list ap)
{
	size_t max_size;
	size_t actual_size;
	uint32_t fn_size;
	char *chunk;
	char *msg_len_pt;
	uint32_t msg_len;
	struct qb_log_target *t = qb_log_target_get(target);

	if (t->instance == NULL) {
		return;
	}

	fn_size = strlen(cs->function) + 1;

	actual_size = 4 * sizeof(uint32_t) + sizeof(uint8_t) + fn_size + sizeof(struct timespec);
	max_size = actual_size + t->max_line_length;

	chunk = qb_rb_chunk_alloc(t->instance, max_size);

	if (chunk == NULL) {
		/* something bad has happened. abort blackbox logging */
		qb_util_perror(LOG_ERR, "Blackbox allocation error, aborting blackbox log %s", t->filename);
		qb_rb_close(qb_rb_lastref_and_ret(
			(struct qb_ringbuffer_s **) &t->instance
		));
		return;
	}

	/* line number */
	memcpy(chunk, &cs->lineno, sizeof(uint32_t));
	chunk += sizeof(uint32_t);

	/* tags */
	memcpy(chunk, &cs->tags, sizeof(uint32_t));
	chunk += sizeof(uint32_t);

	/* log level/priority */
	memcpy(chunk, &cs->priority, sizeof(uint8_t));
	chunk += sizeof(uint8_t);

	/* function name */
	memcpy(chunk, &fn_size, sizeof(uint32_t));
	chunk += sizeof(uint32_t);
	memcpy(chunk, cs->function, fn_size);
	chunk += fn_size;

	/* timestamp */
	memcpy(chunk, timestamp, sizeof(struct timespec));
	chunk += sizeof(struct timespec);

	/* log message length */
	msg_len_pt = chunk;
	chunk += sizeof(uint32_t);

	/* log message */
	msg_len = qb_vsnprintf_serialize(chunk, max_size, cs->format, ap);
	if (msg_len >= max_size) {
	    chunk = msg_len_pt + sizeof(uint32_t); /* Reset */

	    /* Leave this at QB_LOG_MAX_LEN so as not to overflow the blackbox */
	    msg_len = qb_vsnprintf_serialize(chunk, QB_LOG_MAX_LEN,
		"Log message too long to be stored in the blackbox.  "\
		"Maximum is QB_LOG_MAX_LEN" , ap);
	}

	actual_size += msg_len;

	/* now that we know the length, write it
	 */
	memcpy(msg_len_pt, &msg_len, sizeof(uint32_t));

	(void)qb_rb_chunk_commit(t->instance, actual_size);
}