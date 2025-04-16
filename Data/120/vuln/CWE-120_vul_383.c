static unsigned write_erst_record(ERSTDeviceState *s)
{
    unsigned rc = STATUS_FAILED;
    unsigned exchange_length;
    unsigned index;
    uint64_t record_identifier;
    uint32_t record_length;
    uint8_t *exchange;
    uint8_t *nvram = NULL;
    bool record_found = false;

    exchange_length = memory_region_size(&s->exchange_mr);

    /* Validate record_offset */
    if (s->record_offset > (exchange_length - UEFI_CPER_RECORD_MIN_SIZE)) {
        return STATUS_FAILED;
    }

    /* Obtain pointer to record in the exchange buffer */
    exchange = memory_region_get_ram_ptr(&s->exchange_mr);
    exchange += s->record_offset;

    /* Validate CPER record_length */
    memcpy((uint8_t *)&record_length, &exchange[UEFI_CPER_RECORD_LENGTH_OFFSET],
        sizeof(uint32_t));
    record_length = le32_to_cpu(record_length);
    if (record_length < UEFI_CPER_RECORD_MIN_SIZE) {
        return STATUS_FAILED;
    }
    if ((s->record_offset + record_length) > exchange_length) {
        return STATUS_FAILED;
    }

    /* Extract record identifier */
    memcpy((uint8_t *)&record_identifier, &exchange[UEFI_CPER_RECORD_ID_OFFSET],
        sizeof(uint64_t));
    record_identifier = le64_to_cpu(record_identifier);

    /* Check for valid record identifier */
    if (!ERST_IS_VALID_RECORD_ID(record_identifier)) {
        return STATUS_FAILED;
    }

    index = lookup_erst_record(s, record_identifier);
    if (index) {
        /* Record found, overwrite existing record */
        nvram = get_nvram_ptr_by_index(s, index);
        record_found = true;
    } else {
        /* Record not found, not an overwrite, allocate for write */
        index = find_next_empty_record_index(s);
        if (index) {
            nvram = get_nvram_ptr_by_index(s, index);
        } else {
            /* All slots are occupied */
            rc = STATUS_NOT_ENOUGH_SPACE;
        }
    }
    if (nvram) {
        /* Write the record into the slot */
        memcpy(nvram, exchange, record_length);
        memset(nvram + record_length, exchange_length - record_length, 0xFF);
        /* If a new record, increment the record_count */
        if (!record_found) {
            uint32_t record_count;
            record_count = le32_to_cpu(s->header->record_count);
            record_count += 1; /* writing new record */
            s->header->record_count = cpu_to_le32(record_count);
        }
        update_map_entry(s, index, record_identifier);
        rc = STATUS_SUCCESS;
    }

    return rc;
}