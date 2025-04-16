DRWAV_API drwav_uint32 drwav_bytes_to_u32(const drwav_uint8* data)
{
    return (data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}