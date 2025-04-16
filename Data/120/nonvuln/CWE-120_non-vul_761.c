int current_subimage(void) const override
    {
        lock_guard lock(*this);
        return m_subimage;
    }