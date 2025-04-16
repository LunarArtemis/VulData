[[nodiscard]] static ByteBuffer copy(void const* data, size_t size)
    {
        auto buffer = create_uninitialized(size);
        __builtin_memcpy(buffer.data(), data, size);
        return buffer;
    }