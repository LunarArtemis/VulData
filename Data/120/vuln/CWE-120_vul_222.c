bool CrsfParser_TryParseCrsfPacket(CrsfPacket_t *const new_packet, CrsfParserStatistics_t *const parser_statistics)
{
	uint32_t buffer_count;
	uint8_t working_byte;
	uint8_t packet_size;
	uint8_t packet_type;
	bool valid_packet = false;

	buffer_count = QueueBuffer_Count(&rx_queue);

	// Iterate through the buffer to parse the message out
	while ((working_index < buffer_count) && (buffer_count - working_index) >= working_segment_size) {
		switch (parser_state) {
		// Header
		case PARSER_STATE_HEADER:
			if (QueueBuffer_Get(&rx_queue, &working_byte)) {
				if (working_byte == CRSF_HEADER) {
					parser_state = PARSER_STATE_SIZE_TYPE;
					working_segment_size = PACKET_SIZE_TYPE_SIZE;
					working_index = 0;
					buffer_count = QueueBuffer_Count(&rx_queue);
					continue;

				} else {
					parser_statistics->disposed_bytes++;
				}
			}

			working_index = 0;
			working_segment_size = HEADER_SIZE;
			break;

		// Packet size type
		case PARSER_STATE_SIZE_TYPE:
			QueueBuffer_Peek(&rx_queue, working_index++, &packet_size);
			QueueBuffer_Peek(&rx_queue, working_index++, &packet_type);

			working_descriptor = FindCrsfDescriptor((enum CRSF_PACKET_TYPE)packet_type);

			// If we know what this packet is...
			if (working_descriptor != NULL) {
				// Validate length
				if (packet_size != working_descriptor->packet_size + PACKET_SIZE_TYPE_SIZE) {
					parser_statistics->invalid_known_packet_sizes++;
					parser_state = PARSER_STATE_HEADER;
					working_segment_size = HEADER_SIZE;
					working_index = 0;
					buffer_count = QueueBuffer_Count(&rx_queue);
					continue;
				}

				working_segment_size = working_descriptor->packet_size;

			} else {
				// We don't know what this packet is, so we'll let the parser continue
				// just so that we can dequeue it in one shot
				working_segment_size = packet_size + PACKET_SIZE_TYPE_SIZE;

				if (working_segment_size > CRSF_MAX_PACKET_LEN) {
					parser_statistics->invalid_unknown_packet_sizes++;
					parser_state = PARSER_STATE_HEADER;
					working_segment_size = HEADER_SIZE;
					working_index = 0;
					buffer_count = QueueBuffer_Count(&rx_queue);
					continue;
				}
			}

			parser_state = PARSER_STATE_PAYLOAD;
			break;

		// Full packet content
		case PARSER_STATE_PAYLOAD:
			working_index += working_segment_size;
			working_segment_size = CRC_SIZE;
			parser_state = PARSER_STATE_CRC;
			break;

		// CRC
		case PARSER_STATE_CRC:
			// Fetch the suspected packet as a contingous block of memory
			QueueBuffer_PeekBuffer(&rx_queue, 0, process_buffer, working_index + CRC_SIZE);

			// Verify checksum
			if (Crc8Calc(process_buffer + PACKET_SIZE_SIZE, working_index - PACKET_SIZE_SIZE) == process_buffer[working_index]) {
				if (working_descriptor != NULL) {
					if (working_descriptor->processor != NULL) {
						if (working_descriptor->processor(process_buffer + PACKET_SIZE_TYPE_SIZE, working_index - PACKET_SIZE_TYPE_SIZE,
										  new_packet)) {
							parser_statistics->crcs_valid_known_packets++;
							valid_packet = true;
						}
					}

				} else {
					// No working_descriptor at this point means unknown packet
					parser_statistics->crcs_valid_unknown_packets++;
				}

				// Remove the sucessfully processed data from the queue
				QueueBuffer_Dequeue(&rx_queue, working_index + CRC_SIZE);

			} else {
				parser_statistics->crcs_invalid++;
			}

			working_index = 0;
			working_segment_size = HEADER_SIZE;
			parser_state = PARSER_STATE_HEADER;

			if (valid_packet) {
				return true;
			}

			break;
		}

		buffer_count = QueueBuffer_Count(&rx_queue);
	}

	return false;
}