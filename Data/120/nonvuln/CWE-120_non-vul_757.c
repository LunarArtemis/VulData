bool fread(void* buf, size_t itemsize, size_t nitems)
    {
        size_t n = ::fread(buf, itemsize, nitems, m_file);
        if (n != nitems)
            errorf("Read error");
        return n == nitems;
    }