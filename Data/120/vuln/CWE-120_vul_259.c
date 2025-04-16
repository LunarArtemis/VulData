DRWAV_API drwav_uint16 drwav_bytes_to_u16(const drwav_uint8* data)
{
    return (data[0] << 0) | (data[1] << 8);
}