static unsigned read_erst_record(ERSTDeviceState *s)
{
    unsigned rc = STATUS_RECORD_NOT_FOUND;
    unsigned exchange_length;
    unsigned index;

    /* Check if backend storage is empty */
    if (le32_to_cpu(s->header->record_count) == 0) {
        return STATUS_RECORD_STORE_EMPTY;
    }

    exchange_length = memory_region_size(&s->exchange_mr);

    /* Check for record identifier of all 0s */
    if (s->record_identifier == ERST_UNSPECIFIED_RECORD_ID) {
        /* Set to 'first' record in storage */
        get_next_record_identifier(s, &s->record_identifier, true);
        /* record_identifier is now a valid id, or all 1s */
    }

    /* Check for record identifier of all 1s */
    if (s->record_identifier == ERST_EMPTY_END_RECORD_ID) {
        return STATUS_FAILED;
    }

    /* Validate record_offset */
    if (s->record_offset > (exchange_length - UEFI_CPER_RECORD_MIN_SIZE)) {
        return STATUS_FAILED;
    }

    index = lookup_erst_record(s, s->record_identifier);
    if (index) {
        uint8_t *nvram;
        uint8_t *exchange;
        uint32_t record_length;

        /* Obtain pointer to the exchange buffer */
        exchange = memory_region_get_ram_ptr(&s->exchange_mr);
        exchange += s->record_offset;
        /* Obtain pointer to slot in storage */
        nvram = get_nvram_ptr_by_index(s, index);
        /* Validate CPER record_length */
        memcpy((uint8_t *)&record_length,
            &nvram[UEFI_CPER_RECORD_LENGTH_OFFSET],
            sizeof(uint32_t));
        record_length = le32_to_cpu(record_length);
        if (record_length < UEFI_CPER_RECORD_MIN_SIZE) {
            rc = STATUS_FAILED;
        }
        if ((s->record_offset + record_length) > exchange_length) {
            rc = STATUS_FAILED;
        }
        /* If all is ok, copy the record to the exchange buffer */
        if (rc != STATUS_FAILED) {
            memcpy(exchange, nvram, record_length);
            rc = STATUS_SUCCESS;
        }
    } else {
        /*
         * See "Reading : 'The steps performed by the platform ...' 2.c"
         * Set to 'first' record in storage
         */
        get_next_record_identifier(s, &s->record_identifier, true);
    }

    return rc;
}