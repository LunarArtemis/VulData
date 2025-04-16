int publish_decode(u8_t flags, u32_t var_length, struct buf_ctx *buf,
		   struct mqtt_publish_param *param)
{
	int err_code;
	u32_t var_header_length;

	param->dup_flag = flags & MQTT_HEADER_DUP_MASK;
	param->retain_flag = flags & MQTT_HEADER_RETAIN_MASK;
	param->message.topic.qos = ((flags & MQTT_HEADER_QOS_MASK) >> 1);

	err_code = unpack_utf8_str(buf, &param->message.topic.topic);
	if (err_code != 0) {
		return err_code;
	}

	var_header_length = param->message.topic.topic.size + sizeof(u16_t);

	if (param->message.topic.qos > MQTT_QOS_0_AT_MOST_ONCE) {
		err_code = unpack_uint16(buf, &param->message_id);
		if (err_code != 0) {
			return err_code;
		}

		var_header_length += sizeof(u16_t);
	}

	param->message.payload.data = NULL;
	param->message.payload.len = var_length - var_header_length;

	return 0;
}