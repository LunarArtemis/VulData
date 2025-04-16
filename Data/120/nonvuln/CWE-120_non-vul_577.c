static char *int_append_with_semicolon(char *buffer, uint8_t value) {
    // We cheat a little here: for the beauty of initizliaing the above array
    // with a block of text, we manually aligned the data array to 4 to
    // be able to interpret it as uint-array generating fast accesses like
    //    mov eax, DWORD PTR convert_lookup[0+rax*4]
    // (only slightly invokong undefined behavior with this type punning :) )
    const uint32_t *const four_bytes = (const uint32_t *)convert_lookup.data;
    if (value >= 100) {
        memcpy(buffer, &four_bytes[value], 4);
        return buffer + 4;
    }
    if (value >= 10) {
        memcpy(buffer, &four_bytes[value], 4);  // copy 4 cheaper than 3
        return buffer + 3;
    }
    memcpy(buffer, &four_bytes[value], 2);
    return buffer + 2;
}