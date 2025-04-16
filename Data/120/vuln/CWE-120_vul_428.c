static int mqtt_read_message_chunk(struct mqtt_client *client,
				   struct buf_ctx *buf, u32_t length)
{
	int remaining;
	int len;

	/* Calculate how much data we need to read from the transport,
	 * given the already buffered data.
	 */
	remaining = length - (buf->end - buf->cur);
	if (remaining <= 0) {
		return 0;
	}

	/* Check if read does not exceed the buffer. */
	if (buf->end + remaining > client->rx_buf + client->rx_buf_size) {
		MQTT_ERR("[CID %p]: Buffer too small to receive the message",
			 client);
		return -ENOMEM;
	}

	len = mqtt_transport_read(client, buf->end, remaining, false);
	if (len < 0) {
		MQTT_TRC("[CID %p]: Transport read error: %d", client, len);
		return len;
	}

	if (len == 0) {
		MQTT_TRC("[CID %p]: Connection closed.", client);
		return -ENOTCONN;
	}

	client->internal.rx_buf_datalen += len;
	buf->end += len;

	if (len < remaining) {
		MQTT_TRC("[CID %p]: Message partially received.", client);
		return -EAGAIN;
	}

	return 0;
}