size_t count_in_range(size_t start, size_t len, bool value) const
    {
        VERIFY(start < m_size);
        VERIFY(start + len <= m_size);
        if (len == 0)
            return 0;

        static const u8 bitmask_first_byte[8] = { 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80 };
        static const u8 bitmask_last_byte[8] = { 0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F };

        size_t count;
        const u8* first = &m_data[start / 8];
        const u8* last = &m_data[(start + len) / 8];
        u8 byte = *first;
        byte &= bitmask_first_byte[start % 8];
        if (first == last) {
            byte &= bitmask_last_byte[(start + len) % 8];
            count = __builtin_popcount(byte);
        } else {
            count = __builtin_popcount(byte);
            byte = *last;
            byte &= bitmask_last_byte[(start + len) % 8];
            count += __builtin_popcount(byte);
            if (++first < last) {
                const u32* ptr32 = (const u32*)(((FlatPtr)first + sizeof(u32) - 1) & ~(sizeof(u32) - 1));
                if ((const u8*)ptr32 > last)
                    ptr32 = (const u32*)last;
                while (first < (const u8*)ptr32) {
                    count += __builtin_popcount(*first);
                    first++;
                }
                const u32* last32 = (const u32*)((FlatPtr)last & ~(sizeof(u32) - 1));
                while (ptr32 < last32) {
                    count += __builtin_popcountl(*ptr32);
                    ptr32++;
                }
                for (first = (const u8*)ptr32; first < last; first++)
                    count += __builtin_popcount(*first);
            }
        }

        if (!value)
            count = len - count;
        return count;
    }