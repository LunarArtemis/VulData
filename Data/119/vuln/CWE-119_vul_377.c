static inline size_t base64_estimate_decode_size(size_t base64_in_size)
{
    return ((base64_in_size / 4) * 3);
}